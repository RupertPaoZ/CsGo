; ModuleID = 'main'
source_filename = "main"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%retType = type { i32 }
%retType.0 = type { i32 }

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)

define %retType @qsort(i32* %arr, i32 %begin, i32 %end) {
entry:
  %0 = alloca i32*
  store i32* %arr, i32** %0
  %1 = load i32*, i32** %0
  %2 = alloca i32
  store i32 %begin, i32* %2
  %3 = alloca i32
  store i32 %end, i32* %3
  %4 = load i32, i32* %2
  %5 = load i32, i32* %3
  %tmpSLT = icmp slt i32 %4, %5
  br i1 %tmpSLT, label %ifbody, label %end23

ifbody:                                           ; preds = %entry
  %6 = alloca i32
  %7 = alloca i32
  %8 = alloca i32
  %9 = load i32, i32* %2
  store i32 %9, i32* %6
  %10 = load i32, i32* %3
  store i32 %10, i32* %7
  %11 = load i32, i32* %2
  %12 = ptrtoint i32* %1 to i64
  %13 = zext i32 %11 to i64
  %14 = mul i64 %13, 4
  %15 = add i64 %12, %14
  %16 = inttoptr i64 %15 to i32*
  %17 = load i32, i32* %16
  store i32 %17, i32* %8
  br label %cond

cond:                                             ; preds = %end19, %ifbody
  %18 = load i32, i32* %6
  %19 = load i32, i32* %7
  %tmpSLT1 = icmp slt i32 %18, %19
  br i1 %tmpSLT1, label %body, label %end20

body:                                             ; preds = %cond
  br label %cond2

cond2:                                            ; preds = %body4, %body
  %20 = load i32, i32* %7
  %21 = ptrtoint i32* %1 to i64
  %22 = zext i32 %20 to i64
  %23 = mul i64 %22, 4
  %24 = add i64 %21, %23
  %25 = inttoptr i64 %24 to i32*
  %26 = load i32, i32* %25
  %27 = load i32, i32* %8
  %tmpSGT = icmp sgt i32 %26, %27
  %28 = load i32, i32* %6
  %29 = load i32, i32* %7
  %tmpSLT3 = icmp slt i32 %28, %29
  %andtmp = and i1 %tmpSLT3, %tmpSGT
  br i1 %andtmp, label %body4, label %end5

body4:                                            ; preds = %cond2
  %30 = load i32, i32* %7
  %subtmpi = sub i32 %30, 1
  store i32 %subtmpi, i32* %7
  br label %cond2

end5:                                             ; preds = %cond2
  %31 = load i32, i32* %6
  %32 = load i32, i32* %7
  %tmpSLT6 = icmp slt i32 %31, %32
  br i1 %tmpSLT6, label %ifbody7, label %end8

ifbody7:                                          ; preds = %end5
  %33 = load i32, i32* %7
  %34 = ptrtoint i32* %1 to i64
  %35 = zext i32 %33 to i64
  %36 = mul i64 %35, 4
  %37 = add i64 %34, %36
  %38 = inttoptr i64 %37 to i32*
  %39 = load i32, i32* %38
  %40 = load i32, i32* %6
  %41 = ptrtoint i32* %1 to i64
  %42 = zext i32 %40 to i64
  %43 = mul i64 %42, 4
  %44 = add i64 %41, %43
  %45 = inttoptr i64 %44 to i32*
  store i32 %39, i32* %45
  %46 = load i32, i32* %6
  %addtmpi = add i32 %46, 1
  store i32 %addtmpi, i32* %6
  br label %end8

end8:                                             ; preds = %ifbody7, %end5
  br label %cond9

cond9:                                            ; preds = %body13, %end8
  %47 = load i32, i32* %6
  %48 = ptrtoint i32* %1 to i64
  %49 = zext i32 %47 to i64
  %50 = mul i64 %49, 4
  %51 = add i64 %48, %50
  %52 = inttoptr i64 %51 to i32*
  %53 = load i32, i32* %52
  %54 = load i32, i32* %8
  %tmpSLT10 = icmp slt i32 %53, %54
  %55 = load i32, i32* %6
  %56 = load i32, i32* %7
  %tmpSLT11 = icmp slt i32 %55, %56
  %andtmp12 = and i1 %tmpSLT11, %tmpSLT10
  br i1 %andtmp12, label %body13, label %end15

body13:                                           ; preds = %cond9
  %57 = load i32, i32* %6
  %addtmpi14 = add i32 %57, 1
  store i32 %addtmpi14, i32* %6
  br label %cond9

end15:                                            ; preds = %cond9
  %58 = load i32, i32* %6
  %59 = load i32, i32* %7
  %tmpSLT16 = icmp slt i32 %58, %59
  br i1 %tmpSLT16, label %ifbody17, label %end19

ifbody17:                                         ; preds = %end15
  %60 = load i32, i32* %6
  %61 = ptrtoint i32* %1 to i64
  %62 = zext i32 %60 to i64
  %63 = mul i64 %62, 4
  %64 = add i64 %61, %63
  %65 = inttoptr i64 %64 to i32*
  %66 = load i32, i32* %65
  %67 = load i32, i32* %7
  %68 = ptrtoint i32* %1 to i64
  %69 = zext i32 %67 to i64
  %70 = mul i64 %69, 4
  %71 = add i64 %68, %70
  %72 = inttoptr i64 %71 to i32*
  store i32 %66, i32* %72
  %73 = load i32, i32* %7
  %subtmpi18 = sub i32 %73, 1
  store i32 %subtmpi18, i32* %7
  br label %end19

end19:                                            ; preds = %ifbody17, %end15
  br label %cond

end20:                                            ; preds = %cond
  %74 = load i32, i32* %8
  %75 = load i32, i32* %6
  %76 = ptrtoint i32* %1 to i64
  %77 = zext i32 %75 to i64
  %78 = mul i64 %77, 4
  %79 = add i64 %76, %78
  %80 = inttoptr i64 %79 to i32*
  store i32 %74, i32* %80
  %81 = load i32, i32* %2
  %82 = load i32, i32* %6
  %subtmpi21 = sub i32 %82, 1
  %83 = call %retType @qsort(i32* %1, i32 %81, i32 %subtmpi21)
  %84 = load i32, i32* %6
  %addtmpi22 = add i32 %84, 1
  %85 = load i32, i32* %3
  %86 = call %retType @qsort(i32* %1, i32 %addtmpi22, i32 %85)
  br label %end23

end23:                                            ; preds = %end20, %entry
  %87 = alloca %retType
  %88 = getelementptr inbounds %retType, %retType* %87, i32 0, i32 0
  store i32 0, i32* %88
  %89 = load %retType, %retType* %87
  ret %retType %89
}

define %retType.0 @main() {
entry:
  %0 = alloca [10000 x i32]
  %1 = alloca i32
  %2 = alloca i32
  %3 = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @0, i32 0, i32 0), i32* %1)
  store i32 0, i32* %2
  br label %cond

cond:                                             ; preds = %body, %entry
  %4 = load i32, i32* %2
  %5 = load i32, i32* %1
  %tmpSLT = icmp slt i32 %4, %5
  br i1 %tmpSLT, label %body, label %end

body:                                             ; preds = %cond
  %6 = load i32, i32* %2
  %7 = getelementptr inbounds [10000 x i32], [10000 x i32]* %0, i32 0, i32 %6
  %8 = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @1, i32 0, i32 0), i32* %7)
  %9 = load i32, i32* %2
  %addtmpi = add i32 %9, 1
  store i32 %addtmpi, i32* %2
  br label %cond

end:                                              ; preds = %cond
  %10 = getelementptr inbounds [10000 x i32], [10000 x i32]* %0, i32 0, i32 0
  %11 = load i32, i32* %1
  %subtmpi = sub i32 %11, 1
  %12 = call %retType @qsort(i32* %10, i32 0, i32 %subtmpi)
  store i32 0, i32* %2
  br label %cond1

cond1:                                            ; preds = %body3, %end
  %13 = load i32, i32* %2
  %14 = load i32, i32* %1
  %tmpSLT2 = icmp slt i32 %13, %14
  br i1 %tmpSLT2, label %body3, label %end5

body3:                                            ; preds = %cond1
  %15 = load i32, i32* %2
  %16 = getelementptr inbounds [10000 x i32], [10000 x i32]* %0, i32 0, i32 %15
  %17 = load i32, i32* %16
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @2, i32 0, i32 0), i32 %17)
  %19 = load i32, i32* %2
  %addtmpi4 = add i32 %19, 1
  store i32 %addtmpi4, i32* %2
  br label %cond1

end5:                                             ; preds = %cond1
  %20 = alloca %retType.0
  %21 = getelementptr inbounds %retType.0, %retType.0* %20, i32 0, i32 0
  store i32 0, i32* %21
  %22 = load %retType.0, %retType.0* %20
  ret %retType.0 %22
}