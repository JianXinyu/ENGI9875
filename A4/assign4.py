#! /usr/bin/env python3
import random
import sys

if __name__ == '__main__':
    print("For FIFO, LRU, CLOCK: algorithm> Policy CacheSize")
    print("For Belady: algorithm> Policy CacheSize Page List")
    print("{:<8}F -page fault".format('Output: '))
    print("{:<8}NF-no page fault".format(''))
    print("{:<8}E -evit page".format(''))
    param = list(input("algorithm> ").split())
    if len(param) < 2:
        exit(0)
    else:
        policy = param[0]
        cachesize = int(param[1])
        if len(param) == 3:
            # used in optimal policy
            chtsht = param[2].split(',')
                    
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
        # readline can read EOF, don't use input()
        addr = (sys.stdin.readline()).strip('\n')  # reduce \n
        # exit
        if addr == " " or addr == '':
            print("All done, goodbye!")
            exit(0)
        addrList.append(addr)
        print("%s" % addr, end='')

        # 1. look up memory
        n = int(addr)
        try:
            idx = memory.index(n)
            hits = hits + 1
            print('NF')
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
            # 2. page replacement
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
                    for pageIndex in range(0, count):
                        page = memory[pageIndex]
                        # now, have page 'page' at index 'pageIndex' in memory
                        whenReferenced = len(chtsht)
                        # whenReferenced tells us when, in the future, this was referenced
                        for futureIdx in range(len(addrList) + 1, len(addrList)):
                            futurePage = int(chtsht[futureIdx])
                            if page == futurePage:
                                whenReferenced = futureIdx
                                break
                        if whenReferenced >= maxReplace:
                            replaceIdx = pageIndex
                            replacePage = page
                            maxReplace = whenReferenced
                    victim = memory.pop(replaceIdx)
                else:
                    print("Policy %s is undefined!" % policy)
                    exit(1)

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

