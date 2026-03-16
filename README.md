# OurScheme
此專題為中原大學資訊工程系「程式語言」的專題作業<br>
全部專題包含四個部分，此專案時做了前兩個部分<br>

## 專案說明
### Project 1<br>
讀取Input後，判斷該資料是否符合OurScheme的語法<br>
若**不符合**，將**Error種類印出**，並**標示出該Error的位置**<br>
若**符合**，則僅將**Input資料處理成OurScheme的Output形式顯示**。<br>
### Project 2<br>
基於Project 1，讀取Input後，判斷該資料是否符合OurScheme的語法外，還需檢查是否符合各項指令所需的參數格式<br>
若**不符合**，將**Error種類印出**，並**標示出該Error的位置**<br>
若**符合**，則僅將**Input資料根據輸入的指令處理**，**隨後再以OurScheme的Output形式顯示**。<br>

## 開發環境
系統：Windows10<br>
工具：Dev C++、VSCode<br>

## 檔案說明
* OurSchemeProj2-UTF-8.txt 為 Project 2 的詳細說明，包含了各個指令的功能
* P2_Proj2.cpp 為 Project 2 的程式碼
* P2_proj2.exe 為 Project 2 的執行檔
* testInput.txt 為測試的輸入檔案

## 功能
實作了以下十個功能 (後面的數字代表該指令需要的參數數量)
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
輸入完最後一行後，請輸入 Ctrl+z (EOF)作為結尾。
判斷輸入的指令結束後將會輸出結果<br>
