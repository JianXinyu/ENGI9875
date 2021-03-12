#! /usr/bin/env python3
import random
import sys

if __name__ == '__main__':
    param = input("algorithm>")
    policy, cachesize = param.split(" ")
    cachesize = int(cachesize)
    clockbits = 2

    random.seed(10)
    # track reference bits for clock
    ref = {}

    addrList = []
    addrIndex = 0
    idx = 0
    # init memory structure
    count = 0
    memory = []
    hits = 0
    miss = 0
    while True:
        print("page>", end='')
        addr = (sys.stdin.readline()).strip('\n')
        # exit
        if addr == " ":
            print("All done, goodbye!")
            exit(0)
        addrList.append(addr)
        print("%s" % addr, end='')

        # 1. look up memory
        n = int(addr)
        try:
            idx = memory.index(n)
            hits = hits + 1
            if policy == 'LRU' or policy == 'MRU':
                update = memory.remove(n)
                memory.append(n)  # puts it on MRU side
        except:
            # if not in memory
            print("F", end=' ')
            idx = -1
            miss = miss + 1

        victim = -1
        if idx == -1:
            # miss, replace?
            # print('BUG count, cachesize:', count, cachesize)

            if count == cachesize:
                # must replace
                if policy == 'FIFO' or policy == 'LRU':
                    victim = memory.pop(0)
                elif policy == 'MRU':
                    victim = memory.pop(count - 1)
                elif policy == 'CLOCK':
                    victim = -1
                    while victim == -1:
                        # pick up a random page in the circular list
                        page = memory[int(random.random() * count)]
                        if ref[page] >= 1:
                            ref[page] -= 1
                        else:
                            # this is our victim
                            victim = page
                            memory.remove(page)
                            break

                    # remove old page's ref count
                    if page in memory:
                        assert ('BROKEN')
                    del ref[victim]

                elif policy == 'BELADY':
                    maxReplace = -1
                    replaceIdx = -1
                    replacePage = -1
                    # print('OPT: access %d, memory %s' % (n, memory) )
                    # print('OPT: replace from FUTURE (%s)' % addrList[addrIndex+1:])
                    for pageIndex in range(0, count):
                        page = memory[pageIndex]
                        # now, have page 'page' at index 'pageIndex' in memory
                        whenReferenced = len(addrList)
                        # whenReferenced tells us when, in the future, this was referenced
                        for futureIdx in range(addrIndex + 1, len(addrList)):
                            futurePage = int(addrList[futureIdx])
                            if page == futurePage:
                                whenReferenced = futureIdx
                                break
                        # print('OPT: page %d is referenced at %d' % (page, whenReferenced))
                        if whenReferenced >= maxReplace:
                            # print('OPT: ??? updating maxReplace (%d %d %d)' % (replaceIdx, replacePage, maxReplace))
                            replaceIdx = pageIndex
                            replacePage = page
                            maxReplace = whenReferenced
                            # print('OPT: --> updating maxReplace (%d %d %d)' % (replaceIdx, replacePage, maxReplace))
                    victim = memory.pop(replaceIdx)
                else:
                    print("Policy %s is undefined!" % policy)
                    exit(1)
                    # print('OPT: replacing page %d (idx:%d) because I saw it in future at %d' % (victim, replaceIdx, whenReferenced))
                print(" E%d" % victim)
            else:
                # miss, but no replacement needed (cache not full)
                print("")
                victim = -1
                count = count + 1

            # now add to memory
            memory.append(n)
            if victim != -1:
                assert (victim not in memory)

        # after miss processing, update reference bit
        if n not in ref:
            ref[n] = 1
        else:
            ref[n] += 1
            if ref[n] > clockbits:
                ref[n] = clockbits
