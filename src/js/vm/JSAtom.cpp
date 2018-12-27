/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * JS atom table.
 */

#include "vm/JSAtom-inl.h"

#include "mozilla/ArrayUtils.h"
#include "mozilla/EndianUtils.h"
#include "mozilla/RangedPtr.h"
#include "mozilla/Unused.h"

#include <string.h>

#include "jstypes.h"

#include "builtin/String.h"
#include "gc/Marking.h"
#include "js/CharacterEncoding.h"
#include "util/Text.h"
#include "vm/JSContext.h"
#include "vm/SymbolType.h"
#include "vm/Xdr.h"

#include "gc/AtomMarking-inl.h"
#include "vm/JSContext-inl.h"
#include "vm/JSObject-inl.h"
#include "vm/Realm-inl.h"
#include "vm/StringType-inl.h"

using namespace js;

using mozilla::ArrayEnd;
using mozilla::ArrayLength;
using mozilla::Maybe;
using mozilla::Nothing;
using mozilla::RangedPtr;

struct js::AtomHasher::Lookup
{
    union {
        const JS::Latin1Char* latin1Chars;
        const char16_t* twoByteChars;
    };
    bool isLatin1;
    size_t length;
    const JSAtom* atom; /* Optional. */
    JS::AutoCheckCannotGC nogc;

    HashNumber hash;

    MOZ_ALWAYS_INLINE Lookup(const char16_t* chars, size_t length)
      : twoByteChars(chars), isLatin1(false), length(length), atom(nullptr),
        hash(mozilla::HashString(chars, length))
    {}

    MOZ_ALWAYS_INLINE Lookup(const JS::Latin1Char* chars, size_t length)
      : latin1Chars(chars), isLatin1(true), length(length), atom(nullptr),
        hash(mozilla::HashString(chars, length))
    {}

    inline explicit Lookup(const JSAtom* atom)
      : isLatin1(atom->hasLatin1Chars()), length(atom->length()), atom(atom),
        hash(atom->hash())
    {
        if (isLatin1) {
            latin1Chars = atom->latin1Chars(nogc);
            MOZ_ASSERT(mozilla::HashString(latin1Chars, length) == hash);
        } else {
            twoByteChars = atom->twoByteChars(nogc);
            MOZ_ASSERT(mozilla::HashString(twoByteChars, length) == hash);
        }
    }
};

inline HashNumber
js::AtomHasher::hash(const Lookup& l)
{
    return l.hash;
}

MOZ_ALWAYS_INLINE bool
js::AtomHasher::match(const AtomStateEntry& entry, const Lookup& lookup)
{
    JSAtom* key = entry.asPtrUnbarriered();
    if (lookup.atom) {
        return lookup.atom == key;
    }
    if (key->length() != lookup.length || key->hash() != lookup.hash) {
        return false;
    }

    if (key->hasLatin1Chars()) {
        const Latin1Char* keyChars = key->latin1Chars(lookup.nogc);
        if (lookup.isLatin1) {
            return mozilla::ArrayEqual(keyChars, lookup.latin1Chars, lookup.length);
        }
        return EqualChars(keyChars, lookup.twoByteChars, lookup.length);
    }

    const char16_t* keyChars = key->twoByteChars(lookup.nogc);
    if (lookup.isLatin1) {
        return EqualChars(lookup.latin1Chars, keyChars, lookup.length);
    }
    return mozilla::ArrayEqual(keyChars, lookup.twoByteChars, lookup.length);
}

inline JSAtom*
js::AtomStateEntry::asPtr(JSContext* cx) const
{
    JSAtom* atom = asPtrUnbarriered();
    if (!cx->helperThread()) {
        JSString::readBarrier(atom);
    }
    return atom;
}

UniqueChars
js::AtomToPrintableString(JSContext* cx, JSAtom* atom)
{
    return QuoteString(cx, atom);
}

#define DEFINE_PROTO_STRING(name,init,clasp) const char js_##name##_str[] = #name;
JS_FOR_EACH_PROTOTYPE(DEFINE_PROTO_STRING)
#undef DEFINE_PROTO_STRING

#define CONST_CHAR_STR(idpart, id, text) const char js_##idpart##_str[] = text;
FOR_EACH_COMMON_PROPERTYNAME(CONST_CHAR_STR)
#undef CONST_CHAR_STR

// Use a low initial capacity for the permanent atoms table to avoid penalizing
// runtimes that create a small number of atoms.
static const uint32_t JS_PERMANENT_ATOM_SIZE = 64;

MOZ_ALWAYS_INLINE AtomSet::Ptr
js::FrozenAtomSet::readonlyThreadsafeLookup(const AtomSet::Lookup& l) const
{
    return mSet->readonlyThreadsafeLookup(l);
}

struct CommonNameInfo
{
    const char* str;
    size_t length;
};

bool
JSRuntime::initializeAtoms(JSContext* cx)
{
    MOZ_ASSERT(!atoms_);
    MOZ_ASSERT(!permanentAtomsDuringInit_);
    MOZ_ASSERT(!permanentAtoms_);

    if (parentRuntime) {
        permanentAtoms_ = parentRuntime->permanentAtoms_;

        staticStrings = parentRuntime->staticStrings;
        commonNames = parentRuntime->commonNames;
        emptyString = parentRuntime->emptyString;
        wellKnownSymbols = parentRuntime->wellKnownSymbols;

        atoms_ = js_new<AtomsTable>();
        if (!atoms_) {
            return false;
        }

        return atoms_->init();
    }

    permanentAtomsDuringInit_ = js_new<AtomSet>(JS_PERMANENT_ATOM_SIZE);
    if (!permanentAtomsDuringInit_) {
        return false;
    }

    staticStrings = js_new<StaticStrings>();
    if (!staticStrings || !staticStrings->init(cx)) {
        return false;
    }

    static const CommonNameInfo cachedNames[] = {
#define COMMON_NAME_INFO(idpart, id, text) { js_##idpart##_str, sizeof(text) - 1 },
        FOR_EACH_COMMON_PROPERTYNAME(COMMON_NAME_INFO)
#undef COMMON_NAME_INFO
#define COMMON_NAME_INFO(name, init, clasp) { js_##name##_str, sizeof(#name) - 1 },
        JS_FOR_EACH_PROTOTYPE(COMMON_NAME_INFO)
#undef COMMON_NAME_INFO
#define COMMON_NAME_INFO(name) { #name, sizeof(#name) - 1 },
        JS_FOR_EACH_WELL_KNOWN_SYMBOL(COMMON_NAME_INFO)
#undef COMMON_NAME_INFO
#define COMMON_NAME_INFO(name) { "Symbol." #name, sizeof("Symbol." #name) - 1 },
        JS_FOR_EACH_WELL_KNOWN_SYMBOL(COMMON_NAME_INFO)
#undef COMMON_NAME_INFO
    };

    commonNames = js_new<JSAtomState>();
    if (!commonNames) {
        return false;
    }

    ImmutablePropertyNamePtr* names = reinterpret_cast<ImmutablePropertyNamePtr*>(commonNames.ref());
    for (size_t i = 0; i < ArrayLength(cachedNames); i++, names++) {
        JSAtom* atom = Atomize(cx, cachedNames[i].str, cachedNames[i].length, PinAtom);
        if (!atom) {
            return false;
        }
        names->init(atom->asPropertyName());
    }
    MOZ_ASSERT(uintptr_t(names) == uintptr_t(commonNames + 1));

    emptyString = commonNames->empty;

    // Create the well-known symbols.
    wellKnownSymbols = js_new<WellKnownSymbols>();
    if (!wellKnownSymbols) {
        return false;
    }

    ImmutablePropertyNamePtr* descriptions = commonNames->wellKnownSymbolDescriptions();
    ImmutableSymbolPtr* symbols = reinterpret_cast<ImmutableSymbolPtr*>(wellKnownSymbols.ref());
    for (size_t i = 0; i < JS::WellKnownSymbolLimit; i++) {
        JS::Symbol* symbol = JS::Symbol::new_(cx, JS::SymbolCode(i), descriptions[i]);
        if (!symbol) {
            ReportOutOfMemory(cx);
            return false;
        }
        symbols[i].init(symbol);
    }

    return true;
}

void
JSRuntime::finishAtoms()
{
    js_delete(atoms_.ref());

    if (!parentRuntime) {
        js_delete(permanentAtomsDuringInit_.ref());
        js_delete(permanentAtoms_.ref());
        js_delete(staticStrings.ref());
        js_delete(commonNames.ref());
        js_delete(wellKnownSymbols.ref());
    }

    atoms_ = nullptr;
    permanentAtomsDuringInit_ = nullptr;
    permanentAtoms_ = nullptr;
    staticStrings = nullptr;
    commonNames = nullptr;
    wellKnownSymbols = nullptr;
    emptyString = nullptr;
}

class AtomsTable::AutoLock
{
    Mutex* lock = nullptr;

  public:
    MOZ_ALWAYS_INLINE explicit AutoLock(JSRuntime* rt, Mutex& aLock) {
        if (rt->hasHelperThreadZones()) {
            lock = &aLock;
            lock->lock();
        }
    }

    MOZ_ALWAYS_INLINE ~AutoLock() {
        if (lock) {
            lock->unlock();
        }
    }
};

AtomsTable::Partition::Partition(uint32_t index)
  : lock(MutexId { mutexid::AtomsTable.name, mutexid::AtomsTable.order + index }),
    atoms(InitialTableSize),
    atomsAddedWhileSweeping(nullptr)
{}

AtomsTable::Partition::~Partition()
{
    MOZ_ASSERT(!atomsAddedWhileSweeping);
}

AtomsTable::~AtomsTable()
{
    for (size_t i = 0; i < PartitionCount; i++) {
        js_delete(partitions[i]);
    }
}

bool
AtomsTable::init()
{
    for (size_t i = 0; i < PartitionCount; i++) {
        partitions[i] = js_new<Partition>(i);
        if (!partitions[i]) {
            return false;
        }
    }
    return true;
}

void
AtomsTable::lockAll()
{
    MOZ_ASSERT(!allPartitionsLocked);

    for (size_t i = 0; i < PartitionCount; i++) {
        partitions[i]->lock.lock();
    }

#ifdef DEBUG
    allPartitionsLocked = true;
#endif
}

void
AtomsTable::unlockAll()
{
    MOZ_ASSERT(allPartitionsLocked);

    for (size_t i = 0; i < PartitionCount; i++) {
        partitions[PartitionCount - i - 1]->lock.unlock();
    }

#ifdef DEBUG
    allPartitionsLocked = false;
#endif
}

MOZ_ALWAYS_INLINE size_t
AtomsTable::getPartitionIndex(const AtomHasher::Lookup& lookup)
{
    size_t index = lookup.hash >> (32 - PartitionShift);
    MOZ_ASSERT(index < PartitionCount);
    return index;
}

inline void
AtomsTable::tracePinnedAtomsInSet(JSTracer* trc, AtomSet& atoms)
{
    for (auto r = atoms.all(); !r.empty(); r.popFront()) {
        const AtomStateEntry& entry = r.front();
        MOZ_ASSERT(entry.isPinned() == entry.asPtrUnbarriered()->isPinned());
        if (entry.isPinned()) {
            JSAtom* atom = entry.asPtrUnbarriered();
            TraceRoot(trc, &atom, "interned_atom");
            MOZ_ASSERT(entry.asPtrUnbarriered() == atom);
        }
    }
}

void
AtomsTable::tracePinnedAtoms(JSTracer* trc, const AutoAccessAtomsZone& access)
{
    for (size_t i = 0; i < PartitionCount; i++) {
        Partition& part = *partitions[i];
        tracePinnedAtomsInSet(trc, part.atoms);
        if (part.atomsAddedWhileSweeping) {
            tracePinnedAtomsInSet(trc, *part.atomsAddedWhileSweeping);
        }
    }
}

void
js::TraceAtoms(JSTracer* trc, const AutoAccessAtomsZone& access)
{
    JSRuntime* rt = trc->runtime();
    if (rt->permanentAtomsPopulated()) {
        rt->atoms().tracePinnedAtoms(trc, access);
    }
}

static void
TracePermanentAtoms(JSTracer* trc, AtomSet::Range atoms)
{
    for (; !atoms.empty(); atoms.popFront()) {
        const AtomStateEntry& entry = atoms.front();
        JSAtom* atom = entry.asPtrUnbarriered();
        MOZ_ASSERT(atom->isPinned());
        TraceProcessGlobalRoot(trc, atom, "permanent atom");
    }
}

void
JSRuntime::tracePermanentAtoms(JSTracer* trc)
{
    // Permanent atoms only need to be traced in the runtime which owns them.
    if (parentRuntime) {
        return;
    }

    // Static strings are not included in the permanent atoms table.
    if (staticStrings) {
        staticStrings->trace(trc);
    }

    if (permanentAtomsDuringInit_) {
        TracePermanentAtoms(trc, permanentAtomsDuringInit_->all());
    }

    if (permanentAtoms_) {
        TracePermanentAtoms(trc, permanentAtoms_->all());
    }
}

void
js::TraceWellKnownSymbols(JSTracer* trc)
{
    JSRuntime* rt = trc->runtime();

    if (rt->parentRuntime) {
        return;
    }

    if (WellKnownSymbols* wks = rt->wellKnownSymbols) {
        for (size_t i = 0; i < JS::WellKnownSymbolLimit; i++) {
            TraceProcessGlobalRoot(trc, wks->get(i).get(), "well_known_symbol");
        }
    }
}

void
AtomsTable::sweepAll(JSRuntime* rt)
{
    for (size_t i = 0; i < PartitionCount; i++) {
        AutoLock lock(rt, partitions[i]->lock);
        AtomSet& atoms = partitions[i]->atoms;
        for (AtomSet::Enum e(atoms); !e.empty(); e.popFront()) {
            JSAtom* atom = e.front().asPtrUnbarriered();
            if (IsAboutToBeFinalizedUnbarriered(&atom)) {
                e.removeFront();
            }
        }
    }
}

AtomsTable::SweepIterator::SweepIterator(AtomsTable& atoms)
  : atoms(atoms),
    partitionIndex(0)
{
    startSweepingPartition();
    settle();
}

inline void
AtomsTable::SweepIterator::startSweepingPartition()
{
    MOZ_ASSERT(atoms.partitions[partitionIndex]->atomsAddedWhileSweeping);
    atomsIter.emplace(atoms.partitions[partitionIndex]->atoms);
}

inline void
AtomsTable::SweepIterator::finishSweepingPartition()
{
    atomsIter.reset();
    atoms.mergeAtomsAddedWhileSweeping(*atoms.partitions[partitionIndex]);
}

inline void
AtomsTable::SweepIterator::settle()
{
    MOZ_ASSERT(!empty());

    while (atomsIter->empty()) {
        finishSweepingPartition();
        partitionIndex++;
        if (empty()) {
            return;
        }
        startSweepingPartition();
    }
}

inline bool
AtomsTable::SweepIterator::empty() const
{
    return partitionIndex == PartitionCount;
}

inline JSAtom*
AtomsTable::SweepIterator::front() const
{
    MOZ_ASSERT(!empty());
    return atomsIter->front().asPtrUnbarriered();
}

inline void
AtomsTable::SweepIterator::removeFront()
{
    MOZ_ASSERT(!empty());
    return atomsIter->removeFront();
}

inline void
AtomsTable::SweepIterator::popFront()
{
    MOZ_ASSERT(!empty());
    atomsIter->popFront();
    settle();
}

bool
AtomsTable::startIncrementalSweep()
{
    MOZ_ASSERT(JS::RuntimeHeapIsCollecting());

    bool ok = true;
    for (size_t i = 0; i < PartitionCount; i++) {
        auto& part = *partitions[i];

        auto newAtoms = js_new<AtomSet>();
        if (!newAtoms) {
            ok = false;
            break;
        }

        MOZ_ASSERT(!part.atomsAddedWhileSweeping);
        part.atomsAddedWhileSweeping = newAtoms;
    }

    if (!ok) {
        for (size_t i = 0; i < PartitionCount; i++) {
            auto& part = *partitions[i];
            js_delete(part.atomsAddedWhileSweeping);
            part.atomsAddedWhileSweeping = nullptr;
        }
    }

    return ok;
}

void
AtomsTable::mergeAtomsAddedWhileSweeping(Partition& part)
{
    // Add atoms that were added to the secondary table while we were sweeping
    // the main table.

    AutoEnterOOMUnsafeRegion oomUnsafe;

    auto newAtoms = part.atomsAddedWhileSweeping;
    part.atomsAddedWhileSweeping = nullptr;

    for (auto r = newAtoms->all(); !r.empty(); r.popFront()) {
        if (!part.atoms.putNew(AtomHasher::Lookup(r.front().asPtrUnbarriered()), r.front())) {
            oomUnsafe.crash("Adding atom from secondary table after sweep");
        }
    }

    js_delete(newAtoms);
}

bool
AtomsTable::sweepIncrementally(SweepIterator& atomsToSweep, SliceBudget& budget)
{
    // Sweep the table incrementally until we run out of work or budget.
    while (!atomsToSweep.empty()) {
        budget.step();
        if (budget.isOverBudget()) {
            return false;
        }

        JSAtom* atom = atomsToSweep.front();
        if (IsAboutToBeFinalizedUnbarriered(&atom)) {
            atomsToSweep.removeFront();
        }
        atomsToSweep.popFront();
    }

    for (size_t i = 0; i < PartitionCount; i++) {
        MOZ_ASSERT(!partitions[i]->atomsAddedWhileSweeping);
    }

    return true;
}

size_t
AtomsTable::sizeOfIncludingThis(mozilla::MallocSizeOf mallocSizeOf) const
{
    size_t size = sizeof(AtomsTable);
    for (size_t i = 0; i < PartitionCount; i++) {
        size += sizeof(Partition);
        size += partitions[i]->atoms.shallowSizeOfExcludingThis(mallocSizeOf);
    }
    return size;
}

bool
JSRuntime::initMainAtomsTables(JSContext* cx)
{
    MOZ_ASSERT(!parentRuntime);
    MOZ_ASSERT(!permanentAtomsPopulated());

    // The permanent atoms table has now been populated.
    permanentAtoms_ = js_new<FrozenAtomSet>(permanentAtomsDuringInit_); // Takes ownership.
    permanentAtomsDuringInit_ = nullptr;

    // Initialize the main atoms table.
    MOZ_ASSERT(!atoms_);
    atoms_ = js_new<AtomsTable>();
    return atoms_ && atoms_->init();
}

template <typename CharT>
MOZ_NEVER_INLINE static JSAtom*
PermanentlyAtomizeAndCopyChars(JSContext* cx,
                               Maybe<AtomSet::AddPtr>& zonePtr,
                               const CharT* tbchars, size_t length,
                               const Maybe<uint32_t>& indexValue,
                               const AtomHasher::Lookup& lookup);

template <typename CharT>
MOZ_ALWAYS_INLINE static JSAtom*
AllocateNewAtom(JSContext* cx, const CharT* tbchars, size_t length, PinningBehavior pin,
                const Maybe<uint32_t>& indexValue, const AtomHasher::Lookup& lookup);

/* |tbchars| must not point into an inline or short string. */
template <typename CharT>
MOZ_ALWAYS_INLINE
static JSAtom*
AtomizeAndCopyChars(JSContext* cx, const CharT* tbchars, size_t length, PinningBehavior pin,
                    const Maybe<uint32_t>& indexValue)
{
    if (JSAtom* s = cx->staticStrings().lookup(tbchars, length)) {
        return s;
    }

    AtomHasher::Lookup lookup(tbchars, length);

    // Try the per-Zone cache first. If we find the atom there we can avoid the
    // atoms lock, the markAtom call, and the multiple HashSet lookups below.
    // We don't use the per-Zone cache if we want a pinned atom: handling that
    // is more complicated and pinning atoms is relatively uncommon.
    Zone* zone = cx->zone();
    Maybe<AtomSet::AddPtr> zonePtr;
    if (MOZ_LIKELY(zone && pin == DoNotPinAtom)) {
        zonePtr.emplace(zone->atomCache().lookupForAdd(lookup));
        if (zonePtr.ref()) {
            // The cache is purged on GC so if we're in the middle of an
            // incremental GC we should have barriered the atom when we put
            // it in the cache.
            JSAtom* atom = zonePtr.ref()->asPtrUnbarriered();
            MOZ_ASSERT(AtomIsMarked(zone, atom));
            return atom;
        }
    }

    // This function can be called during initialization, while the permanent
    // atoms table is being created. In this case all atoms created are added to
    // the permanent atoms table.
    if (!cx->permanentAtomsPopulated()) {
        return PermanentlyAtomizeAndCopyChars(cx, zonePtr, tbchars, length, indexValue, lookup);
    }

    AtomSet::Ptr pp = cx->permanentAtoms().readonlyThreadsafeLookup(lookup);
    if (pp) {
        JSAtom* atom = pp->asPtr(cx);
        if (zonePtr &&
            MOZ_UNLIKELY(!zone->atomCache().add(*zonePtr, AtomStateEntry(atom, false))))
        {
            ReportOutOfMemory(cx);
            return nullptr;
        }

        return atom;
    }

    // Validate the length before taking an atoms partition lock, as throwing an
    // exception here may reenter this code.
    if (MOZ_UNLIKELY(!JSString::validateLength(cx, length))) {
        return nullptr;
    }

    JSAtom* atom = cx->atoms().atomizeAndCopyChars(cx,
                                                   tbchars, length,
                                                   pin,
                                                   indexValue,
                                                   lookup);
    if (!atom) {
        return nullptr;
    }

    cx->atomMarking().inlinedMarkAtom(cx, atom);

    if (zonePtr &&
        MOZ_UNLIKELY(!zone->atomCache().add(*zonePtr, AtomStateEntry(atom, false))))
    {
        ReportOutOfMemory(cx);
        return nullptr;
    }

    return atom;
}

template <typename CharT>
MOZ_ALWAYS_INLINE
JSAtom*
AtomsTable::atomizeAndCopyChars(JSContext* cx,
                                const CharT* tbchars, size_t length,
                                PinningBehavior pin,
                                const Maybe<uint32_t>& indexValue,
                                const AtomHasher::Lookup& lookup)
{
    Partition& part = *partitions[getPartitionIndex(lookup)];
    AutoLock lock(cx->runtime(), part.lock);

    AtomSet& atoms = part.atoms;
    AtomSet* atomsAddedWhileSweeping = part.atomsAddedWhileSweeping;
    AtomSet::AddPtr p;

    if (!atomsAddedWhileSweeping) {
        p = atoms.lookupForAdd(lookup);
    } else {
        // We're currently sweeping the main atoms table and all new atoms will
        // be added to a secondary table. Check this first.
        p = atomsAddedWhileSweeping->lookupForAdd(lookup);

        // If that fails check the main table but check if any atom found there
        // is dead.
        if (!p) {
            if (AtomSet::AddPtr p2 = atoms.lookupForAdd(lookup)) {
                JSAtom* atom = p2->asPtrUnbarriered();
                if (!IsAboutToBeFinalizedUnbarriered(&atom)) {
                    p = p2;
                }
            }
        }
    }

    if (p) {
        JSAtom* atom = p->asPtr(cx);
        if (pin && !atom->isPinned()) {
            atom->setPinned();
            p->setPinned(true);
        }
        return atom;
    }

    JSAtom* atom = AllocateNewAtom(cx, tbchars, length, pin, indexValue, lookup);
    if (!atom) {
        return nullptr;
    }

    // We have held the lock since looking up p, and the operations we've done
    // since then can't GC; therefore the atoms table has not been modified and
    // p is still valid.
    AtomSet* addSet = part.atomsAddedWhileSweeping ? part.atomsAddedWhileSweeping : &atoms;
    if (MOZ_UNLIKELY(!addSet->add(p, AtomStateEntry(atom, bool(pin))))) {
        ReportOutOfMemory(cx); /* SystemAllocPolicy does not report OOM. */
        return nullptr;
    }

    return atom;
}

template JSAtom*
AtomizeAndCopyChars(JSContext* cx, const char16_t* tbchars, size_t length, PinningBehavior pin,
                    const Maybe<uint32_t>& indexValue);

template JSAtom*
AtomizeAndCopyChars(JSContext* cx, const Latin1Char* tbchars, size_t length, PinningBehavior pin,
                    const Maybe<uint32_t>& indexValue);

template <typename CharT>
MOZ_NEVER_INLINE static JSAtom*
PermanentlyAtomizeAndCopyChars(JSContext* cx,
                               Maybe<AtomSet::AddPtr>& zonePtr,
                               const CharT* tbchars, size_t length,
                               const Maybe<uint32_t>& indexValue,
                               const AtomHasher::Lookup& lookup)
{
    MOZ_ASSERT(!cx->permanentAtomsPopulated());
    MOZ_ASSERT(CurrentThreadCanAccessRuntime(cx->runtime()));

    JSRuntime* rt = cx->runtime();
    AtomSet& atoms = *rt->permanentAtomsDuringInit();
    AtomSet::AddPtr p = atoms.lookupForAdd(lookup);
    if (p) {
        return p->asPtr(cx);
    }

    JSAtom* atom = AllocateNewAtom(cx, tbchars, length, DoNotPinAtom, indexValue, lookup);
    if (!atom) {
        return nullptr;
    }

    atom->morphIntoPermanentAtom();

    // We are single threaded at this point, and the operations we've done since
    // then can't GC; therefore the atoms table has not been modified and p is
    // still valid.
    if (!atoms.add(p, AtomStateEntry(atom, true))) {
        ReportOutOfMemory(cx); /* SystemAllocPolicy does not report OOM. */
        return nullptr;
    }

    if (zonePtr &&
        MOZ_UNLIKELY(!cx->zone()->atomCache().add(*zonePtr, AtomStateEntry(atom, false))))
    {
        ReportOutOfMemory(cx);
        return nullptr;
    }

    return atom;
}

template <typename CharT>
MOZ_ALWAYS_INLINE static JSAtom*
AllocateNewAtom(JSContext* cx, const CharT* tbchars, size_t length, PinningBehavior pin,
                const Maybe<uint32_t>& indexValue, const AtomHasher::Lookup& lookup)
{
    AutoAllocInAtomsZone ac(cx);

    JSFlatString* flat = NewStringCopyN<NoGC>(cx, tbchars, length);
    if (!flat) {
        // Grudgingly forgo last-ditch GC. The alternative would be to release
        // the lock, manually GC here, and retry from the top. If you fix this,
        // please also fix or comment the similar case in Symbol::new_.
        ReportOutOfMemory(cx);
        return nullptr;
    }

    JSAtom* atom = flat->morphAtomizedStringIntoAtom(lookup.hash);
    MOZ_ASSERT(atom->hash() == lookup.hash);

    if (pin) {
        atom->setPinned();
    }

    if (indexValue) {
        atom->maybeInitializeIndex(*indexValue, true);
    }

    return atom;
}

JSAtom*
js::AtomizeString(JSContext* cx, JSString* str,
                  js::PinningBehavior pin /* = js::DoNotPinAtom */)
{
    if (str->isAtom()) {
        JSAtom& atom = str->asAtom();
        /* N.B. static atoms are effectively always interned. */
        if (pin == PinAtom && !atom.isPinned()) {
            cx->runtime()->atoms().pinExistingAtom(cx, &atom);
        }

        return &atom;
    }

    JSLinearString* linear = str->ensureLinear(cx);
    if (!linear) {
        return nullptr;
    }

    Maybe<uint32_t> indexValue;
    if (str->hasIndexValue()) {
        indexValue.emplace(str->getIndexValue());
    }

    JS::AutoCheckCannotGC nogc;
    return linear->hasLatin1Chars()
           ? AtomizeAndCopyChars(cx, linear->latin1Chars(nogc), linear->length(), pin, indexValue)
           : AtomizeAndCopyChars(cx, linear->twoByteChars(nogc), linear->length(), pin, indexValue);
}

void
AtomsTable::pinExistingAtom(JSContext* cx, JSAtom* atom)
{
    MOZ_ASSERT(atom);
    MOZ_ASSERT(!atom->isPinned());

    AtomHasher::Lookup lookup(atom);

    AtomsTable::Partition& part = *partitions[getPartitionIndex(lookup)];
    AtomsTable::AutoLock lock(cx->runtime(), part.lock);
    AtomSet::Ptr p = part.atoms.lookup(lookup);
    if (!p && part.atomsAddedWhileSweeping) {
        p = part.atomsAddedWhileSweeping->lookup(lookup);
    }

    MOZ_ASSERT(p); // Unpinned atoms must exist in atoms table.
    MOZ_ASSERT(p->asPtrUnbarriered() == atom);

    atom->setPinned();
    p->setPinned(true);
}

JSAtom*
js::Atomize(JSContext* cx, const char* bytes, size_t length, PinningBehavior pin,
            const Maybe<uint32_t>& indexValue)
{
    CHECK_THREAD(cx);

    const Latin1Char* chars = reinterpret_cast<const Latin1Char*>(bytes);
    return AtomizeAndCopyChars(cx, chars, length, pin, indexValue);
}

template <typename CharT>
JSAtom*
js::AtomizeChars(JSContext* cx, const CharT* chars, size_t length, PinningBehavior pin)
{
    CHECK_THREAD(cx);
    return AtomizeAndCopyChars(cx, chars, length, pin, Nothing());
}

template JSAtom*
js::AtomizeChars(JSContext* cx, const Latin1Char* chars, size_t length, PinningBehavior pin);

template JSAtom*
js::AtomizeChars(JSContext* cx, const char16_t* chars, size_t length, PinningBehavior pin);

JSAtom*
js::AtomizeUTF8Chars(JSContext* cx, const char* utf8Chars, size_t utf8ByteLength)
{
    // This could be optimized to hand the char16_t's directly to the JSAtom
    // instead of making a copy. UTF8CharsToNewTwoByteCharsZ should be
    // refactored to take an JSContext so that this function could also.

    UTF8Chars utf8(utf8Chars, utf8ByteLength);

    size_t length;
    UniqueTwoByteChars chars(JS::UTF8CharsToNewTwoByteCharsZ(cx, utf8, &length).get());
    if (!chars) {
        return nullptr;
    }

    return AtomizeChars(cx, chars.get(), length);
}

bool
js::IndexToIdSlow(JSContext* cx, uint32_t index, MutableHandleId idp)
{
    MOZ_ASSERT(index > JSID_INT_MAX);

    char16_t buf[UINT32_CHAR_BUFFER_LENGTH];
    RangedPtr<char16_t> end(ArrayEnd(buf), buf, ArrayEnd(buf));
    RangedPtr<char16_t> start = BackfillIndexInCharBuffer(index, end);

    JSAtom* atom = AtomizeChars(cx, start.get(), end - start);
    if (!atom) {
        return false;
    }

    idp.set(JSID_FROM_BITS((size_t)atom | JSID_TYPE_STRING));
    return true;
}

template <AllowGC allowGC>
static JSAtom*
ToAtomSlow(JSContext* cx, typename MaybeRooted<Value, allowGC>::HandleType arg)
{
    MOZ_ASSERT(!arg.isString());

    Value v = arg;
    if (!v.isPrimitive()) {
        MOZ_ASSERT(!cx->helperThread());
        if (!allowGC) {
            return nullptr;
        }
        RootedValue v2(cx, v);
        if (!ToPrimitive(cx, JSTYPE_STRING, &v2)) {
            return nullptr;
        }
        v = v2;
    }

    if (v.isString()) {
        JSAtom* atom = AtomizeString(cx, v.toString());
        if (!allowGC && !atom) {
            cx->recoverFromOutOfMemory();
        }
        return atom;
    }
    if (v.isInt32()) {
        JSAtom* atom = Int32ToAtom(cx, v.toInt32());
        if (!allowGC && !atom) {
            cx->recoverFromOutOfMemory();
        }
        return atom;
    }
    if (v.isDouble()) {
        JSAtom* atom = NumberToAtom(cx, v.toDouble());
        if (!allowGC && !atom) {
            cx->recoverFromOutOfMemory();
        }
        return atom;
    }
    if (v.isBoolean()) {
        return v.toBoolean() ? cx->names().true_ : cx->names().false_;
    }
    if (v.isNull()) {
        return cx->names().null;
    }
    if (v.isSymbol()) {
        MOZ_ASSERT(!cx->helperThread());
        if (allowGC) {
            JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr,
                                      JSMSG_SYMBOL_TO_STRING);
        }
        return nullptr;
    }
#ifdef ENABLE_BIGINT
    if (v.isBigInt()) {
        JSAtom* atom = BigIntToAtom(cx, v.toBigInt());
        if (!allowGC && !atom) {
            cx->recoverFromOutOfMemory();
        }
        return atom;
    }
#endif
    MOZ_ASSERT(v.isUndefined());
    return cx->names().undefined;
}

template <AllowGC allowGC>
JSAtom*
js::ToAtom(JSContext* cx, typename MaybeRooted<Value, allowGC>::HandleType v)
{
    if (!v.isString()) {
        return ToAtomSlow<allowGC>(cx, v);
    }

    JSString* str = v.toString();
    if (str->isAtom()) {
        return &str->asAtom();
    }

    JSAtom* atom = AtomizeString(cx, str);
    if (!atom && !allowGC) {
        MOZ_ASSERT_IF(!cx->helperThread(), cx->isThrowingOutOfMemory());
        cx->recoverFromOutOfMemory();
    }
    return atom;
}

template JSAtom*
js::ToAtom<CanGC>(JSContext* cx, HandleValue v);

template JSAtom*
js::ToAtom<NoGC>(JSContext* cx, const Value& v);

template<XDRMode mode>
XDRResult
js::XDRAtom(XDRState<mode>* xdr, MutableHandleAtom atomp)
{
    bool latin1 = false;
    uint32_t length = 0;
    uint32_t lengthAndEncoding = 0;
    if (mode == XDR_ENCODE) {
        static_assert(JSString::MAX_LENGTH <= INT32_MAX, "String length must fit in 31 bits");
        latin1 = atomp->hasLatin1Chars();
        length = atomp->length();
        lengthAndEncoding = (length << 1) | uint32_t(latin1);
    }

    MOZ_TRY(xdr->codeUint32(&lengthAndEncoding));

    if (mode == XDR_DECODE) {
        length = lengthAndEncoding >> 1;
        latin1 = lengthAndEncoding & 0x1;
    }

    if (mode == XDR_ENCODE) {
        JS::AutoCheckCannotGC nogc;
        if (latin1) {
            return xdr->codeChars(const_cast<JS::Latin1Char*>(atomp->latin1Chars(nogc)),
                                  length);
        }
        return xdr->codeChars(const_cast<char16_t*>(atomp->twoByteChars(nogc)), length);
    }

    MOZ_ASSERT(mode == XDR_DECODE);
    /* Avoid JSString allocation for already existing atoms. See bug 321985. */
    JSContext* cx = xdr->cx();
    JSAtom* atom;
    if (latin1) {
        const Latin1Char* chars = nullptr;
        if (length) {
            const uint8_t *ptr;
            size_t nbyte = length * sizeof(Latin1Char);
            MOZ_TRY(xdr->peekData(&ptr, nbyte));
            chars = reinterpret_cast<const Latin1Char*>(ptr);
        }
        atom = AtomizeChars(cx, chars, length);
    } else {
#if MOZ_LITTLE_ENDIAN
        /* Directly access the little endian chars in the XDR buffer. */
        const char16_t* chars = nullptr;
        if (length) {
            // In the |mode == XDR_ENCODE| case above, when |nchars > 0|,
            // |XDRState::codeChars(char16_t*, size_t nchars)| will align the
            // buffer.  This code never calls that function, but it must act
            // *as if* it had, so we must align manually here.
            MOZ_TRY(xdr->codeAlign(sizeof(char16_t)));

            const uint8_t* ptr;
            size_t nbyte = length * sizeof(char16_t);
            MOZ_TRY(xdr->peekData(&ptr, nbyte));
            MOZ_ASSERT(reinterpret_cast<uintptr_t>(ptr) % sizeof(char16_t) == 0,
                       "non-aligned buffer during JSAtom decoding");
            chars = reinterpret_cast<const char16_t*>(ptr);
        }
        atom = AtomizeChars(cx, chars, length);
#else
        /*
         * We must copy chars to a temporary buffer to convert between little and
         * big endian data.
         */
        char16_t* chars;
        char16_t stackChars[256];
        UniqueTwoByteChars heapChars;
        if (length <= ArrayLength(stackChars)) {
            chars = stackChars;
        } else {
            /*
             * This is very uncommon. Don't use the tempLifoAlloc arena for this as
             * most allocations here will be bigger than tempLifoAlloc's default
             * chunk size.
             */
            heapChars.reset(cx->pod_malloc<char16_t>(length));
            if (!heapChars) {
                return xdr->fail(JS::TranscodeResult_Throw);
            }

            chars = heapChars.get();
        }

        MOZ_TRY(xdr->codeChars(chars, length));
        atom = AtomizeChars(cx, chars, length);
#endif /* !MOZ_LITTLE_ENDIAN */
    }

    if (!atom) {
        return xdr->fail(JS::TranscodeResult_Throw);
    }
    atomp.set(atom);
    return Ok();
}

template XDRResult
js::XDRAtom(XDRState<XDR_ENCODE>* xdr, MutableHandleAtom atomp);

template XDRResult
js::XDRAtom(XDRState<XDR_DECODE>* xdr, MutableHandleAtom atomp);

Handle<PropertyName*>
js::ClassName(JSProtoKey key, JSContext* cx)
{
    return ClassName(key, cx->names());
}

js::AutoLockAllAtoms::AutoLockAllAtoms(JSRuntime* rt)
  : runtime(rt)
{
    MOZ_ASSERT(CurrentThreadCanAccessRuntime(runtime));
    if (runtime->hasHelperThreadZones()) {
        runtime->atoms().lockAll();
    }
}

js::AutoLockAllAtoms::~AutoLockAllAtoms()
{
    MOZ_ASSERT(CurrentThreadCanAccessRuntime(runtime));
    if (runtime->hasHelperThreadZones()) {
        runtime->atoms().unlockAll();
    }
}
