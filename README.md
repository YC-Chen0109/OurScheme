# OurScheme
此專題為中原大學資訊工程系「程式語言」的專題作業<br>
全部專題包含四個部分，而在這裡放上第兩個部分<br>
Project 1<br>
這個部分的功能為，判斷輸入資料的語法是否符合簡單的Scheme語法<br>
並將所讀到資料用容易看的方式印出來<br>
Project 2<br>
實作個個指令的功能，並將指令執行的結果印出來<br>

## 開發平台
Windows10

## 開發環境
Dev C++

## 檔案說明
* OurSchemeProj2-UTF-8.txt 為 Project 2 的詳細說明，包含了各個指令的功能
* P2_0606.cpp 為 Project 2 的程式碼
* P2_0606.exe 為 Project 2 的執行檔
* testData 為測試的輸入檔案

## 功能
實作了以下十個功能
1. Constructors<br>
cons (2)<br>
list (>= 0)

2. Bypassing the default evaluation<br>
quote (1)<br>
'     (1)

3. The binding of a symbol to an S-expression<br>
define (2)

4. Part accessors<br>
car (1)<br>
cdr (1)

5. Primitive predicates (all functions below can only take 1 argument)<br>
  atom?<br>
  pair?<br>
  list?<br>
  null?<br>
  integer?<br>
  real?<br>
  number?<br>
  string?<br>
  boolean?<br>
  symbol?

6. Basic arithmetic, logical and string operations<br>
  \+ (>= 2)<br>
  \- (>= 2)<br>
  \* (>= 2)<br>
  / (>= 2)<br>
  not (1)<br>
  and (>= 2)<br>
  or  (>= 2)<br>
  \><br>
  \>=<br>
  <<br>
  <=<br>
  =<br>
  string-append<br>
  string>?<br>
  string<?<br>
  string=?

7. Eqivalence tester<br>
  eqv?    (2)<br>
  equal?  (2)

8. Sequencing and functional composition<br>
  begin   (>= 1)

9. Conditionals<br>
  if     (2 or 3)<br>
  cond   (>= 1)

10. clean-environment<br>
  clean-environment   (0)

## 使用方法
先輸入一個整數作為測試數據的編號<br>
接著就可以開始輸入想執行的指令<br>
在程式判斷輸入的指令結束後將會輸出結果<br>
輸入"(exit)"可以結束程式
