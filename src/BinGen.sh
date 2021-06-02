filename=$(basename "$1" .ll)
llc -filetype=obj $filename.ll  -o $filename.o
clang $filename.o -o $filename.out