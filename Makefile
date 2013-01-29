default: debug

all: debug release

.PHONY: all release err oneerr clean debug

debug:
	mkdir -p debug; cd debug && cmake -DCMAKE_BUILD_TYPE=debug -G "Unix Makefiles" -Wdev .. && make -j1

release:
	mkdir -p release; cd release && cmake -DCMAKE_BUILD_TYPE=release -G "Unix Makefiles" -Wdev .. && make -j1

err:
	make 2>&1 | vim -

oneerr:
	make 2>&1 | grep -B 2 'error:' | head -n 3

clean:
	rm -f -R debug release

run:
	./debug/leveldb.json

run_slave:
	./debug/leveldb.json --replication.id=asdasd --port=22501 --db=testdb2

