cd ..
source ./activate
cd userprog
make clean
make
cd build
#pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/exec-once:exec-once -p tests/userprog/child-imple:child-simple -- -q   -f run exec-once

#pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/exec-once:exec-once -p tests/userprog/child-simple:child-simple -- -q   -f run exec-once < /dev/null 2> tests/userprog/exec-once.errors > tests/userprog/exec-once.output
#perl -I../.. ../../tests/userprog/exec-once.ck tests/userprog/exec-once tests/userprog/exec-once.result
pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/fork-once:fork-once -- -q   -f run fork-once < /dev/null 2> tests/userprog/fork-once.errors > tests/userprog/fork-once.output
perl -I../.. ../../tests/userprog/fork-once.ck tests/userprog/fork-once tests/userprog/fork-once.result
pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/fork-multiple:fork-multiple -- -q   -f run fork-multiple < /dev/null 2> tests/userprog/fork-multiple.errors > tests/userprog/fork-multiple.output
perl -I../.. ../../tests/userprog/fork-multiple.ck tests/userprog/fork-multiple tests/userprog/fork-multiple.result
pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/fork-recursive:fork-recursive -- -q   -f run fork-recursive < /dev/null 2> tests/userprog/fork-recursive.errors > tests/userprog/fork-recursive.output
perl -I../.. ../../tests/userprog/fork-recursive.ck tests/userprog/fork-recursive tests/userprog/fork-recursive.result
pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/fork-read:fork-read -p ../../tests/userprog/sample.txt:sample.txt -- -q   -f run fork-read < /dev/null 2> tests/userprog/fork-read.errors > tests/userprog/fork-read.output
perl -I../.. ../../tests/userprog/fork-read.ck tests/userprog/fork-read tests/userprog/fork-read.result
pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/fork-close:fork-close -p ../../tests/userprog/sample.txt:sample.txt -- -q   -f run fork-close < /dev/null 2> tests/userprog/fork-close.errors > tests/userprog/fork-close.output
perl -I../.. ../../tests/userprog/fork-close.ck tests/userprog/fork-close tests/userprog/fork-close.result
pintos -v -k -T 60 -m 20   --fs-disk=10 -p tests/userprog/fork-boundary:fork-boundary -- -q   -f run fork-boundary < /dev/null 2> tests/userprog/fork-boundary.errors > tests/userprog/fork-boundary.output
perl -I../.. ../../tests/userprog/fork-boundary.ck tests/userprog/fork-boundary tests/userprog/fork-boundary.result
