#filename=$(basename "$1" .ll)
# filename="../test/course_assist/test"
filename="../test/course_assist/c_assist2"
echo $filename
llc -filetype=obj $filename.ll  -o $filename.o
clang $filename.o -o $filename.out