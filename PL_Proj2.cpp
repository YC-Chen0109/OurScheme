# include <stdio.h>  // scanf
# include <string.h>
# include <vector>  // vector, push_back
# include <iostream>  // cout, endl
# include <iomanip>  // 小數位數
# include <stdlib.h>  // exit

using namespace std;
static int uTestNum = 0 ;

typedef char Str200[200] ;


struct Token {
  Str200 type ;
  Str200 data ;
  float float_data ;
  int int_data ;
  int column ;
  int line ;
  bool inputline_last;
  bool is_string ;
  bool in_s_exp ;
  bool out_s_exp;
  bool error_tag;
  bool produce_flag;
  bool quote_flag;
  bool is_keep;
  bool error_tree;
};

struct Tree_node {
  Str200 type;
  Token root;
  Tree_node* left;
  Tree_node* right;
};

typedef Tree_node* Tree_pointer;

struct Binding {
  Str200 name;
  bool is_function;
  Tree_pointer meaning ;
  vector<Binding> function_arguement;
  Tree_pointer function_type ;
  Tree_pointer function_body ;
} ;

// struct Function {
//   Str200 name;
//   vector<Binding> arguement;
//   Tree_pointer meaning;
// } ;

class Token_tool {
  public:
  vector<Token> minput_line ; // 存放input line ;
  vector<Token> mtoken_list; // 存放token
  // string peek_token ; // 目前正在scan的token (未放進vector)
  char mpeek_char ; // 目前get到的char (未放進string)

  void Skip_white_space( char& peek_char ) {
    if ( peek_char == ' ' || peek_char == '\t' || peek_char == '\n' ) {
      peek_char = getchar() ;
    } // if() 如果是空白字元就get下一個

  } // Skip_white_space() 跳過空白字元

  bool Skip_white_space( Str200 peek_char, int& i ) {
    while ( peek_char[i+1] == ' ' || peek_char[i+1] == '\t' || peek_char[i+1] == '\n' ) {
      if ( ( i + 1 ) < 100 )
        i++ ;
      else
        return false ; 
    } // while() 如果是空白字元就看下一個index

    return true ;
  } // Skip_white_space() 跳過空白字元 (input_to_token用)

  void Skip_no_print( char& peek_char ) {
    peek_char = getchar() ;
    while ( peek_char != '\n' ) {
      peek_char = getchar() ;
    } // while() 只要不是換行就一直讀

  } // Skip_no_print() 跳過前兩行

  void Read_input( vector<Token>& input_line, vector<Token>& token_list, char& peek_char ) {
    Token peek_token ; // 目前正在scan的token (未放進vector) 
    Str200 peek_string ; // 目前正在組合的string (未放進token)
    int left_paren = 0;
    peek_char = getchar() ;
    while ( peek_char != EOF ) {
      for ( int i = 0 ; peek_char != '\n' && peek_char != EOF ; i++ ) {
        peek_string[i] = peek_char ;
        peek_char = getchar() ;
      } // for() 一直讀input直到遇到換行

      strcpy( peek_token.data, peek_string );
      minput_line.push_back( peek_token ) ; 
      for ( int i = 0 ; peek_string[i] != '\0' && i < 200 ; i++ ) {
        peek_string[i] = '\0' ;
      } // for()

      peek_char = getchar() ;
    } // while()

    for ( int i = 0 ; i < input_line.size() ; i++ ) {
      Input_to_token( token_list, input_line, input_line[i].data, left_paren ) ;
    } // for() 從input_line分割token

  } // Read_input() 讀input

  void Clear_char_pointer( Str200 unclear_char ) {
    for ( int i = 0 ; unclear_char[i] != '\0' && i < 200 ; i++ ) {
      unclear_char[i] = '\0' ;
    } // for()
  } // Clear_char_pointer() 清除用過的字串

  void Init_token( Token& token ) {
    Clear_char_pointer( token.data );
    Clear_char_pointer( token.type );
    token.float_data = 0.0;
    token.int_data = 0;
    token.line = 1 ;
    token.inputline_last = false;
    token.is_string = false;
    token.in_s_exp = false;
    token.out_s_exp = false;
    token.error_tag = false;
    token.produce_flag = false;
    token.quote_flag = false;
    token.is_keep = false;
    token.error_tree = false;
  } // Init_token() 初始化token用

  void Input_to_token( vector<Token>& token_list, vector<Token>& input_line,
                       Str200 peek_string, int& left_paren ) {
    int column = 1;
    int space = 0;
    Token peek_token ;
    Str200 cut_token = {'\0'}; // 目前切下來的部分
    Init_token( peek_token ) ;
    peek_token.is_string = false ;
    Clear_char_pointer( peek_token.type ) ;
    for ( int i = 0 ; i < strlen( peek_string )  ; i++ ) {
      if ( ( peek_string[i] == ' ' || peek_string[i] == '\t' || peek_string[i] == '\n' ) 
           && strlen( cut_token ) != 0 ) {
        strcpy( peek_token.data, cut_token ) ;
        peek_token.column = column - strlen( cut_token ) ;
        token_list.push_back( peek_token ) ;
        // cout << i << " : " << peek_string[i-1] << " paren : " << left_paren << endl << endl;
        if ( left_paren == 0 )
          column = 1;
        peek_token.column = 0;
        Clear_char_pointer( cut_token ) ;
        Clear_char_pointer( peek_token.data ) ;
        // Skip_white_space( peek_string, i ) ;

      } // if() 遇到空白
      else if ( peek_string[i] == '(' ) {
        if ( strlen( cut_token ) != 0 ) {
          strcpy( peek_token.data, cut_token ) ;
          peek_token.column = column - strlen( cut_token ) ;
          token_list.push_back( peek_token ) ;
          Clear_char_pointer( cut_token ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          strcpy( peek_token.data, "(" ) ; // 分割字元複製到token的data 
          if ( left_paren == 0 )
            peek_token.column = 1 ;
          peek_token.column = column;
          token_list.push_back( peek_token ) ;
          left_paren++;
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          // Skip_white_space( peek_string, i ) ;

        } // if() 如果遇到分割符號時還有未存入的token
        else {
          strcpy( peek_token.data, "(" ) ;
          peek_token.column = column;
          token_list.push_back( peek_token ) ;
          left_paren++;
          Clear_char_pointer( peek_token.data ) ;
          // Skip_white_space( peek_string, i ) ;

        } // else() 沒有未存入的token
      } // else if() 遇到左括弧
      else if ( peek_string[i] == ')' ) {
        if ( strlen( cut_token ) != 0 ) {
          strcpy( peek_token.data, cut_token ) ;
          peek_token.column = column - strlen( cut_token ) ;
          token_list.push_back( peek_token ) ;
          Clear_char_pointer( cut_token ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          strcpy( peek_token.data, ")" ) ; // 分割字元複製到token的data 
          peek_token.column = column ;
          token_list.push_back( peek_token ) ;
          if ( left_paren > 0 )
            left_paren--;
          if ( left_paren == 0 )
            column = 0 ;

          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          // Skip_white_space( peek_string, i ) ;

        } // if() 如果遇到分割符號時還有未存入的token
        else {
          strcpy( peek_token.data, ")" ) ;
          peek_token.column = column ;
          token_list.push_back( peek_token ) ;
          if ( left_paren > 0 )
            left_paren--;
          if ( left_paren == 0 )
            column = 0 ;

          Clear_char_pointer( peek_token.data ) ;
          // Skip_white_space( peek_string, i ) ;

        } // else() 沒有未存入的token
      } // else if() 遇到右括弧
      else if ( peek_string[i] == '\'' ) {
        if ( strlen( cut_token ) != 0 ) {
          strcpy( peek_token.data, cut_token ) ;
          peek_token.column = column - strlen( cut_token ) ;
          token_list.push_back( peek_token ) ;
          Clear_char_pointer( cut_token ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          strcpy( peek_token.data, "'" ) ; // 分割字元複製到token的data 
          peek_token.column = 1 ;
          token_list.push_back( peek_token ) ;


          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          // Skip_white_space( peek_string, i ) ;

        } // if() 如果遇到分割符號時還有未存入的token
        else {
          strcpy( peek_token.data, "'" ) ;
          peek_token.column = column ;
          token_list.push_back( peek_token ) ;


          Clear_char_pointer( peek_token.data ) ;
          // Skip_white_space( peek_string, i ) ;

        } // else() 沒有未存入的token
      } // else if() 遇到單quote
      else if ( peek_string[i] == '\"' ) {
        if ( strlen( cut_token ) != 0 ) {
          strcpy( peek_token.data, cut_token ) ;
          peek_token.column = column - strlen( cut_token ) ;
          token_list.push_back( peek_token ) ;
          if ( left_paren == 0 )
            column = 1;
          Clear_char_pointer( cut_token ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          // cut_token[ strlen( cut_token ) ] = peek_string[i] ;
          i++ ;
          column++;
          while ( ( peek_string[i] != '\"' && peek_string[i] != '\n' && i < strlen( peek_string ) ) 
                  || ( i > 0 && peek_string[i-1] == '\\' ) ) {
            cut_token[ strlen( cut_token ) ] = peek_string[i] ;
            i++ ;
            column++;
          } // while() 把雙quote後的string全部拼起來成一token

          // cut_token[ strlen( cut_token ) ] = peek_string[i] ;
          strcpy( peek_token.data, cut_token ) ;
          strcpy( peek_token.type, "STRING" ) ;
          if ( peek_string[i] == '\"' )
            peek_token.is_string = true ;
          peek_token.column = column;
          token_list.push_back( peek_token ) ;
          if ( left_paren == 0 ) {
            column = 0;
            peek_token.column = 0 ;
          } // if()

          Clear_char_pointer( cut_token ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          Clear_char_pointer( peek_token.type ) ;
          // Skip_white_space( peek_string, i ) ;
          peek_token.is_string = false ;
        } // if() 如果遇到分割符號時還有未存入的token
        else {
          // cut_token[ strlen( cut_token ) ] = peek_string[i] ;
          i++ ;
          column++;
          while ( ( peek_string[i] != '\"' && peek_string[i] != '\n' && i < strlen( peek_string ) ) 
                  || ( i > 0 && peek_string[i-1] == '\\' ) ) {
            cut_token[ strlen( cut_token ) ] = peek_string[i] ;
            i++ ;
            column++;
          } // while() 把雙quote後的string全部拼起來成一token

          // cut_token[ strlen( cut_token ) ] = peek_string[i] ;

          strcpy( peek_token.data, cut_token ) ;
          strcpy( peek_token.type, "STRING" ) ;
          if ( peek_string[i] == '\"' )
            peek_token.is_string = true ;
          peek_token.column = column;
          token_list.push_back( peek_token ) ;
          if ( left_paren == 0 ) {
            column = 0;
            peek_token.column = 0 ;
          } // if()
          
          Clear_char_pointer( cut_token ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          Clear_char_pointer( peek_token.type ) ;
          // Skip_white_space( peek_string, i ) ;
          peek_token.is_string = false ;
        } // else() 沒有未存入的token
      } // else if() 遇到雙quote
      else if ( peek_string[i] == ';' ) {
        if ( strlen( cut_token ) != 0 ) {
          strcpy( peek_token.data, cut_token ) ;
          peek_token.column = column - strlen( cut_token ) ;
          token_list.push_back( peek_token ) ;
          column = 0 ;
          Clear_char_pointer( cut_token ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          strcpy( peek_token.data, "-1" ) ;
          strcpy( peek_token.type, "COMMENT" ) ;
          peek_token.line = 1;
          // token_list.push_back( peek_token ) ;
          Clear_char_pointer( peek_token.type ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          i = strlen( peek_string ) + 1 ;
        } // if() 如果遇到分號時還有未存入的token
        else {
          column = 0;
          strcpy( peek_token.data, "-1" ) ;
          strcpy( peek_token.type, "COMMENT" ) ;
          peek_token.line = 1;
          // token_list.push_back( peek_token ) ;
          Clear_char_pointer( peek_token.type ) ; // 清除分割字元前已存入token vector的內容
          Clear_char_pointer( peek_token.data ) ; // 清除已存入vector的token內容
          i = strlen( peek_string ) + 1 ;
        } // else() 沒有未存入的token
      } // else if() 遇到分號 (後面皆為註解可跳過)
      else {
        if ( ( peek_string[i] != ' ' && peek_string[i] != '\t' && peek_string[i] != '\n' ) ) {
          cut_token[ strlen( cut_token ) ] = peek_string[i] ;
        } // if()
      } // else() 沒遇到分割字元

      column++;
    } // for() 切token

    if ( !peek_token.is_string && strlen( cut_token ) > 0 ) {
      strcpy( peek_token.data, cut_token ) ;
      peek_token.column = column - strlen( cut_token );
      token_list.push_back( peek_token ) ;
      Clear_char_pointer( peek_token.data );
      peek_token.is_string = false ;
    } // if() 剛剛切的token不是string

    // strcpy( peek_token.data, "\n" ) ; // 用來分隔vector內不同line的input
    if ( token_list.size() > 0 ) {
      int last = token_list.size() - 1 ;
      token_list[last].inputline_last = true;
    } // if ()

    strcpy( peek_token.type, "endline" ) ; // 用來分隔vector內不同line的input
    peek_token.column = 0;
    token_list.push_back( peek_token ) ;
  } // Input_to_token() 把一行input切成好幾個token

  float Data_to_float( Str200 token_string, int start_index, bool negetive ) {
    float sum = 0.0 ;
    int after_dot_index = 0 ; // 紀錄小數點後有機位
    bool after_dot = false ;
    for ( int i = start_index ; i < strlen( token_string ) ; i++ ) {
      sum = sum * 10 ; // 空出新的位置用於填補新的位數
      if ( token_string[i] == '.' ) {
        after_dot = true ; // 確認到小數點位置，之後的數字都位於小數點後
        sum = sum * 0.1 ; // 此位數用於放置小數點，非有效數字
      } // if()
      else if ( !after_dot )
        sum = sum + ( token_string[i] - '0' ) ;
      else {
        sum = sum + ( token_string[i] - '0' ) ;
        after_dot_index++ ;
      } // else() 新增新位數，同時記錄小數點後有幾位
    } // for() 將data換成數字

    for ( ; after_dot_index > 0 ; after_dot_index-- ) {
      sum = sum * 0.1 ;
    } // for() 小數位數修正

    if ( negetive )
      return sum * -1 ;
    else
      return sum ;
  }  // Data_to_float() 將資料轉為浮點數

  int Data_to_int( Str200 token_string, int start_index, bool negetive ) {
    int sum = 0 ;
    for ( int i = start_index ; i < strlen( token_string ) ; i++ ) {
      sum = sum * 10 ; // 空出新的位置用於填補新的位數
      sum = sum + ( token_string[i] - '0' ) ;
    } // for() 將data換成數字

    if ( negetive )
      return sum * -1 ;
    else
      return sum ;
  }  // Data_to_int() 將資料轉為整數

  bool Check_exit( vector<Token>& token_list, int index, Str200 exit_string ) {
    if ( strcmp( token_list[index].data, "(" ) == 0 ) {
      int current = index+1;
      while ( strcmp( token_list[current].type, "endline" ) == 0 
              && current < token_list.size() ) {
        current++;
      } // while()

      int current2 =  current+1;
      while ( strcmp( token_list[current2].type, "endline" ) == 0 
              && current2 < token_list.size() ) {
        current2++;
      } // while()

      if ( current == token_list.size() )
        current--;
      if ( current2 == token_list.size() )
        current2--;
      if ( strcmp( token_list[current].data, "exit" ) == 0 
           && strcmp( token_list[current2].data, ")" ) == 0 ) {
        return true;
      } // if()
      else
        return false;
    } // if()
    else if ( strcmp( exit_string, "(exit)" ) == 0 || strcmp( exit_string, "( exit )" ) == 0 )
      return true ;
    else
      return false ;
  } // Check_exit() 確認是否離開

  bool Check_nil( Str200 token_string, Token& peek_token ) { 
    if ( strcmp( token_string, "#f" ) == 0 || strcmp( token_string, "()" ) == 0 
         || strcmp( token_string, "nil" ) == 0 ) {
      strcpy( peek_token.type, "NIL" ) ;
      strcpy( peek_token.data, "nil" ) ;
      return true ;
    } // if() 該token為NIL
    else
      return false ;
  } // Check_nil()

  bool Check_t( Str200 token_string, Token& peek_token ) { 
    if ( strcmp( token_string, "#t" ) == 0 || strcmp( token_string, "t" ) == 0 ) {
      strcpy( peek_token.type, "T" ) ;
      strcpy( peek_token.data, "#t" ) ;
      return true ;
    } // if() 該token為T
    else
      return false ;
  } // Check_t()

  bool Check_comment( Str200 token_string, Token& peek_token ) { 
    if ( token_string[0] == ';' ) {
      strcpy( peek_token.type, "COMMENT" ) ;
      return true ;
    } // if() 該token為T
    else
      return false ;
  } // Check_comment()

  bool Check_float_or_int( Str200 token_string, Token& peek_token ) {
    if ( token_string[0] == '.' && strlen( token_string ) == 1 )
      return false;
    if ( token_string[0] == '+' || token_string[0] == '-' ) {
      if ( token_string[1] == '.' && strlen( token_string ) < 3 )
        return false ;
      if ( ( token_string[1] == '+' || token_string[1] == '-' 
             || token_string[1] > '9' || token_string[1] < '0' ) 
           && token_string[1] != '.' )
        return false ;
      else {
        int dot_counter = 0 ;
        for ( int i = 1 ; i < strlen( token_string ) ; i++ ) {
          if ( token_string[i] == '.' )
            dot_counter++ ; // 如果token內有dot，計數器+1
          if ( ( token_string[i] > '9' || token_string[i] < '0' ) && token_string[i] != '.' )
            return false ; // 如果token內出現了不是數字也不是dot的字元，可判斷該token不為float或int
        } // for()

        if ( dot_counter > 1 )
          return false ; // 如果token內dot數量多於1個，判斷部為浮點數or整數
        else if ( dot_counter == 1 ) {
          Clear_char_pointer( peek_token.type );
          strcpy( peek_token.type, "FLOAT" ) ;
          if ( token_string[0] == '-' )
            peek_token.float_data = Data_to_float( token_string, 1, true ) ;
          else
            peek_token.float_data = Data_to_float( token_string, 1, false ) ;
        } // else if() token為浮點數
        else {
          Clear_char_pointer( peek_token.type );
          strcpy( peek_token.type, "INT" ) ;
          if ( token_string[0] == '-' )
            peek_token.int_data = Data_to_int( token_string, 1, true ) ;
          else
            peek_token.int_data = Data_to_int( token_string, 1, false ) ;
        } // else() token為整數

        return true;
      } // else()
    } // if() 如果token開頭為+-
    else if ( ( token_string[0] <= '9' && token_string[0] >= '0' ) || token_string[0] == '.' ) {
      int dot_counter = 0 ;
      for ( int i = 0 ; i < strlen( token_string ) ; i++ ) {
        if ( token_string[i] == '.' )
          dot_counter++ ; // 如果token內有dot，計數器+1
        if ( ( token_string[i] > '9' || token_string[i] < '0' ) && token_string[i] != '.' )
          return false ; // 如果token內出現了不是數字也不是dot的字元，可判斷該token不為float或int
      } // for()

      if ( dot_counter > 1 )
        return false ; // 如果token內dot數量多於1個，判斷部為浮點數or整數
      else if ( dot_counter == 1 ) {
        Clear_char_pointer( peek_token.type );
        strcpy( peek_token.type, "FLOAT" ) ;
        peek_token.float_data = Data_to_float( token_string, 0, false ) ;
      } // else if() token為浮點數
      else {
        Clear_char_pointer( peek_token.type );
        strcpy( peek_token.type, "INT" ) ;
        peek_token.int_data = Data_to_int( token_string, 0, false ) ;
      } // else() token為整數

      return true;
    } // else if() 如果token開頭為數字
    else 
      return false ;
  } // Check_float_or_int() 確認該token是不是float或int

  void Sort_token_type( Str200 &token_string, Token& peek_token ) {
    // cout << token_string << endl ;
    if ( strcmp( peek_token.type, "" ) == 0 ) {
      if ( Check_nil( token_string, peek_token ) )
        ;
      else if ( Check_t( token_string, peek_token ) )
        ;
      else if ( Check_comment( token_string, peek_token ) )
        ;
      else if ( Check_float_or_int( token_string, peek_token ) )
        ;
      else if ( peek_token.is_string )
        strcpy( peek_token.type, "STRING" ) ;
      else if ( strcmp( peek_token.data, "." ) == 0 )
        strcpy( peek_token.type, "DOT" ) ;
      else if ( strcmp( peek_token.data, "(" ) == 0 )
        strcpy( peek_token.type, "LEFT-PAREN" ) ;
      else if ( strcmp( peek_token.data, ")" ) == 0 )
        strcpy( peek_token.type, "RIGHT-PAREN" ) ;
      else if ( strcmp( peek_token.data, "\'" ) == 0 )
        strcpy( peek_token.type, "QUOTE" ) ;
      else
        strcpy( peek_token.type, "SYMBOL" ) ;
    } // if() 如果該token還沒有type
  } // Sort_token_type() token分類

  void Clean_token_list( vector<Token>& token_list ) {
    for ( int index = 0 ; index < token_list.size() ; index++ ) {
      if ( strlen( token_list[index].data ) == 0 
           && !token_list[index].is_string ) {
        token_list.erase( token_list.begin() + index ) ;
        index-- ;
      } // if() 沒內容
    } // for()
  } // Clean_token_list()

  void Set_line_number( vector<Token>& token_list ) {
    int line = 1;
    int paren_number = 0;
    bool legal_token = false ;
    for ( int i = 0 ; i < token_list.size() ; i++ ) {
      if ( strcmp( token_list[i].type, "COMMENT" ) == 0 ) {
        ;
      } // if()
      else if ( strcmp( token_list[i].type, "endline" ) == 0 )
        line++;
      else if ( strcmp( token_list[i].type, "LEFT-PAREN" ) == 0 ) {
        paren_number++;
        token_list[i].line = line;
        legal_token = true;
      } // else if()
      else if ( strcmp( token_list[i].type, "no closing quote" ) == 0 
                || strcmp( token_list[i].type, "Unexpected_ERROR" ) == 0
                || strcmp( token_list[i].type, "Unexpected_ERROR_2" ) == 0 ) {
        token_list[i].line = line;
        legal_token = false;
        while ( strcmp( token_list[i].type, "endline" ) != 0 )
          i++;
        paren_number = 0;
        line = 1;
      } // else if()
      else {
        if ( strcmp( token_list[i].type, "RIGHT-PAREN" ) == 0 )
          paren_number--;
        token_list[i].line = line;
        if ( paren_number == 0 ) {
          if ( token_list[i].inputline_last )
            line = 0;
          else
            line = 1;
        } // if()

        legal_token = true;
      } // else()
    } // for()
  } // Set_line_number()

  void Set_column_number( vector<Token>& token_list ) {
    int paren_number = 0;
    bool legal_token = false ;
    for ( int i = 0 ; i < token_list.size() ; i++ ) {
      if ( strcmp( token_list[i].type, "COMMENT" ) == 0 ) {
        ;
      } // if()
      else if ( strcmp( token_list[i].type, "endline" ) == 0 )
        ;
      else if ( strcmp( token_list[i].type, "LEFT-PAREN" ) == 0 ) {
        paren_number++;
        legal_token = true;
      } // else if()
      // else if ( strcmp( token_list[i].type, "no closing quote" ) == 0 ) {
      //   if ( paren_number == 0 && legal_token 
      //        && ( token_list[i].column > token_list[i-1].column ) )
      //     token_list[i].column = token_list[i].column - token_list[i-1].column 
      //                          - strlen( token_list[i-1].data );
      //   if ( strcmp( token_list[i].data, "\"" ) == 0 )
      //     token_list[i].column++;
      //   paren_number = 0;
      // } // else if
      else if ( strcmp( token_list[i].type, "Unexpected_ERROR" ) == 0 
                || strcmp( token_list[i].type, "Unexpected_ERROR_2" ) == 0 ) {
        if ( paren_number == 0 && legal_token 
             && ( token_list[i].column > token_list[i-1].column ) 
             && strcmp( token_list[i-1].type, "endline" ) != 0 ) {
          token_list[i].column = token_list[i].column - token_list[i-1].column 
                                 - strlen( token_list[i-1].data ) ;
          token_list[i].column++;
        } // if()

        // if ( token_list[i].column == strlen( token_list[i-1].data ) 
        //      && strcmp( token_list[i-1].data, "expr" ) == 0 )
        //   token_list[i].column = 1;

        legal_token = false;

        while ( strcmp( token_list[i].type, "endline" ) != 0 )
          i++;
        

        paren_number = 0;
      } // else if()
      else {
        if ( strcmp( token_list[i].type, "RIGHT-PAREN" ) == 0 )
          paren_number--;
        legal_token = true;
      } // else()
    } // for()
  } // Set_column_number()

};

class Parser_tool {
  public:
  Token_tool mtool;
  void Parser_token( vector<Token>& token_list ) {
    Token insert_token ;
    bool quote_insert = false ; // 用來記錄有沒有因為quote而插入括號的紀錄
    for ( int index = 0 ; index < token_list.size() ; index++ ) {

      if ( strcmp( token_list[index].data, "(" ) == 0 ) {
        int current = index+1;
        while ( strcmp( token_list[current].type, "endline" ) == 0 
                && current < token_list.size() ) {
          current++;
        } // while()

        int current2 =  current+1;
        while ( strcmp( token_list[current2].type, "endline" ) == 0 
                && current2 < token_list.size() ) {
          current2++;
        } // while()

        if ( current == token_list.size() )
          current--;
        if ( current2 == token_list.size() )
          current2--;

        if ( strcmp( token_list[current].type, "RIGHT-PAREN" ) == 0 ) {
          mtool.Clear_char_pointer( token_list[index].data ) ;
          strcpy( token_list[index].data, "nil" ) ;
          mtool.Clear_char_pointer( token_list[index].type ) ;
          strcpy( token_list[index].type, "NIL" ) ;
          token_list.erase( token_list.begin() + current ) ;
        } // if() 該line內的token可拼成nil
        // else if ( strcmp( token_list[current].data, "exit" ) == 0 
        //          && strcmp( token_list[current2].data, ")" ) == 0 ) {
        //  mtool.Clear_char_pointer( token_list[index].data ) ;
        //  strcpy( token_list[index].data, "(exit)" ) ;
        //  mtool.Clear_char_pointer( token_list[index].type ) ;
        //  strcpy( token_list[index].type, "SYMBOL" ) ;
        //  token_list.erase( token_list.begin() + current ) ;
        //  token_list.erase( token_list.begin() + current2 ) ;
        // } // else if()
        // else {
        //  if ( index == 0 || strcmp( token_list[index-1].data, "." ) != 0 ) {
        //    token_list[index].in_s_exp = true ;
        //    index = Paren_parser( token_list, index+1, index, false, false );
        //    token_list[index].out_s_exp = true ;
        //  } // if()
        //  else {
        //    index = Paren_parser( token_list, index, index, true, false );
        //    index-- ;
        //  } // else()
        // } // else() 表示此括號內有s-exp*/
      } // if() 找到左括號的token
    } // for()
  } // Parser_token() 拼接token
  
  int Paren_parser( vector<Token>& token_list, int index, bool have_dot ) {
    int i = index ;
    for ( ; strcmp( token_list[i].data, ")" ) != 0 ; i++ ) {
      // for ( int j = 0 ; j < token_list.size() ; j++ ){
      //  cout << token_list[j].data << " " ;
      // }
      // cout << i << "peek data :" << token_list[i].data << endl << endl ;
      if ( strcmp( token_list[i].data, "(" ) == 0 ) {
        // int current = i+1;
        // while ( strcmp( token_list[current].type, "endline" ) == 0 
        //        && current < token_list.size() ) {
        //  current++;
        // } // while()

        // if ( current == token_list.size() )
        //  current--;

        // if ( strcmp( token_list[current].data, ")" ) == 0 ) {
        //  strcpy( token_list[i].data, "nil" ) ;
        //  mtool.Clear_char_pointer( token_list[index].type ) ;
        //  strcpy( token_list[i].type, "NIL" ) ;
        //  token_list.erase( token_list.begin() + current ) ;
        // } // if() 遇到的不是s-exp而是nil
        // else {
        i++;
        // cout << i << "paren in :" << token_list[i].data << endl << endl ;
        i = Paren_parser( token_list, i, false ) ;
        // cout << i << "paren data :" << token_list[i].data << endl << endl ;
        // i--;
        // } // else()
      } // if() 遇到左括號
      else if ( strcmp( token_list[i].data, "." ) == 0 ) {
        int current = i+1;
        while ( ( strcmp( token_list[current].type, "endline" ) == 0 
                  || strcmp( token_list[current].type, "COMMENT" ) == 0 )
                && current < token_list.size() ) {
          current++;
        } // while()

        if ( current == token_list.size() )
          current--;

        if ( strcmp( token_list[current].data, "(" ) == 0 ) {
          // if ( strcmp( token_list[current2].data, ")" ) == 0 ) {
          //  token_list.erase( token_list.begin() + i ) ;
          //  token_list.erase( token_list.begin() + current ) ;
          //  token_list.erase( token_list.begin() + current2 ) ;
          //  i-- ;
          // } // if() 遇到的不是s-exp而是nil
          // else {
          token_list.erase( token_list.begin() + i ) ;
          token_list.erase( token_list.begin() + current - 1 ) ;
            // cout << "DOT in" << endl ;
          // cout << i << "dot in :" << token_list[i].data << endl << endl ;
          i = Paren_parser( token_list, i, true ) ;
          // cout << i << "dot data :" << token_list[i].data << endl << endl ;
          i-- ;
          // } // else()
        } // if() DOT後面為LEFT-PAREN，只取括弧內的s-exp
        else if ( strcmp( token_list[current].type, "T" ) == 0 
                  || strcmp( token_list[current].type, "NIL" ) == 0 ) {
          token_list.erase( token_list.begin() + i ) ;
          token_list.erase( token_list.begin() + current - 1 ) ;
          i--;
        } // else if() 遇到DOT後面接NIL或T的情況 (直接刪掉)
        else {
          // strcat( token_list[change_data_index].data, " " );
          // strcat( token_list[change_data_index].data, token_list[i].data );
          // token_list.erase( token_list.begin() + i ) ;
          // i-- ;
        } // else() 組合後面的s-exp
      } // else if() 遇到dot-paren
      else {
        ;
      } // else() 組合s-exp
      // cout << "after :" << endl ;
      // for ( int j = 0 ; j < token_list.size() ; j++ ){
      //  cout << token_list[j].data << " " ;
      // }
      // cout << endl;
    } // for()

    // if ( !have_dot ) {
    //  strcat( token_list[change_data_index].data, " " );
    //  strcat( token_list[change_data_index].data, token_list[i].data );
    // } // if()
    if ( have_dot )
      token_list.erase( token_list.begin() + i ) ;
    // mtool.Clear_char_pointer( token_list[index].type );
    // strcpy( token_list[index].type, "S-exp" ) ;

    // if ( uTestNum == 1 ) {
    //   cout << change_data_index << " " << token_list[change_data_index].data << endl ;
    // } // if()
    // cout << i << "return data :" << token_list[i].data << endl << endl ;
    return i;
  } // Paren_parser() 括弧內的s-exp組合

};

class Print_tool {
  public:
  bool Check_need_translate( Str200 print_string ) {
    for ( int i = 0 ; i < 100 ; i++ ) {
      if ( print_string[i] == '\\' )
        return true ;
    } // for()

    return false ;
  } // Check_need_translate() 檢查裡面是否有反斜線

  void Translate_print( Str200 print_string ) {
    for ( int i = 0 ; i < strlen( print_string ) ; i++ ) {
      if ( print_string[i] == '\\' ) {
        if ( print_string[i+1] == 'n' )
          cout << '\n' ;
        else if ( print_string[i+1] == 't' )
          cout << '\t' ;
        else if ( print_string[i+1] != '\'' && print_string[i+1] != '\"' && print_string[i+1] != '\\' ) {
          cout << print_string[i] ;
          i-- ;
        } // else if()
        else
          cout << print_string[i+1] ;
        i++ ;
      } // if() 如果token內有反斜線
      else
        cout << print_string[i] ;
    } // for()
  } // Translate_print()

  void Print_quote_s_exp( vector<Token>& token_list, int& index, int print_space, int left_paren ) {
    cout << " quote" << endl ;
    index++;
    if ( strcmp( token_list[index-1].type, "LEFT-PAREN" ) != 0 
         && strcmp( token_list[index].type, "endline" ) != 0 
         && strcmp( token_list[index].type, "COMMENT" ) != 0 ) {
      for ( int space = print_space ; space > 0 ; space-- ) {
        cout << " ";
      } // for()
    } // if() 前一個不是左括號

    if ( strcmp( token_list[index].type, "FLOAT" ) == 0 ) {
      cout << " " << fixed <<  setprecision( 3 ) << token_list[index].float_data << endl ;
    } // if()
    else if ( strcmp( token_list[index].type, "INT" ) == 0 ) {
      cout << " " << token_list[index].int_data << endl ;
    } // else if()
    else if ( strcmp( token_list[index].type, "endline" ) == 0 )
      ; // 掃到換行標記token
    else if ( strcmp( token_list[index].type, "COMMENT" ) == 0 )
      ; // 掃到換行標記token
    else if ( strcmp( token_list[index].type, "QUOTE" ) == 0 ) {
      Print_quote_s_exp( token_list, index, print_space+2, left_paren+1 );
    } // else if()
    else if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
      Print_s_exp( token_list, index, print_space+2, left_paren+1 );
    } // else if() 要顯示s-exp
    else {
      if ( Check_need_translate( token_list[index].data ) ) {
        cout << " " ;
        Translate_print( token_list[index].data ) ;
        cout << endl;
      } // if() token內有特殊字元要轉譯
      else
        cout << " " << token_list[index].data << endl;
    } // else() token type不是int跟float

    left_paren--;
    if ( left_paren == 0 )
      cout << ")" ;
    else {
      for ( int space = print_space - 1 ; space > 0 ; space-- ) {
        cout << " ";
      } // for() 根據pretty print 印出空格

      cout << ")" << endl ;
    } // else() 如果不是最外圍的s-exp的結尾右括號
  } // Print_quote_s_exp()

  void Print_s_exp( vector<Token>& token_list, int& index, int print_space, int left_paren ) {
    cout << " " << token_list[index].data ;
    bool print_paren = true;
    index++;
    for ( ; strcmp( token_list[index].type, "RIGHT-PAREN" ) != 0 ; index++ ) {

      if ( !print_paren 
           && strcmp( token_list[index].type, "endline" ) != 0 
           && strcmp( token_list[index].type, "COMMENT" ) != 0 ) {
        for ( int space = print_space ; space > 0 ; space-- ) {
          cout << " ";
        } // for()
      } // if() 前一個不是左括號

      if ( strcmp( token_list[index].type, "FLOAT" ) == 0 ) {
        cout << " " << fixed <<  setprecision( 3 ) << token_list[index].float_data << endl ;
        print_paren = false;
      } // if()
      else if ( strcmp( token_list[index].type, "INT" ) == 0 ) {
        cout << " " << token_list[index].int_data << endl ;
        print_paren = false;
      } // else if()
      else if ( strcmp( token_list[index].type, "endline" ) == 0 )
        ; // 掃到換行標記token
      else if ( strcmp( token_list[index].type, "COMMENT" ) == 0 )
        ; // 掃到換行標記token
      else if ( strcmp( token_list[index].type, "QUOTE" ) == 0 ) {
        Print_quote_s_exp( token_list, index, print_space+2, left_paren+1 );
        print_paren = false;
      } // else if()
      else if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
        Print_s_exp( token_list, index, print_space+2, left_paren+1 );
        print_paren = false;
      } // else if() 要顯示s-exp
      else {
        if ( Check_need_translate( token_list[index].data ) ) {
          cout << " " ;
          Translate_print( token_list[index].data ) ;
          cout << endl;
        } // if() token內有特殊字元要轉譯
        else
          cout << " " << token_list[index].data << endl;
        print_paren = false;
      } // else() token type不是int跟float
    } // for()

    left_paren--;
    if ( left_paren == 0 )
      cout << token_list[index].data ;
    else {
      for ( int space = print_space - 1 ; space > 0 ; space-- ) {
        cout << " ";
      } // for() 根據pretty print 印出空格

      cout << token_list[index].data << endl ;
    } // else() 如果不是最外圍的s-exp的結尾右括號
  } // Print_s_exp()
  
  void Print_error_unexpected( Token& token ) {
    cout << "ERROR (unexpected token) : atom or '(' expected when token at"; 
    cout << " Line " << token.line << " Column " << token.column << " is ";
    cout << ">>" << token.data << "<<" << endl ;
  } // Print_error_unexpected()

  void Print_error_unexpected_2( Token& token ) {
    cout << "ERROR (unexpected token) : ')' expected when token at"; 
    cout << " Line " << token.line << " Column " << token.column << " is ";
    cout << ">>" << token.data << "<<" << endl ;
  } // Print_error_unexpected_2()

  void Print_error_no_closing_quote( Token& token ) {
    cout << "ERROR (no closing quote) : END-OF-LINE encountered at"; 
    cout << " Line " << token.line << " Column " << token.column << endl ;
  } // Print_error_no_closing_quote()

  void Print_error_token( Str200 error_type, Token& error_token ) {
    if ( strcmp( error_type, "no closing quote" ) == 0 )
      Print_error_no_closing_quote( error_token );
    else if ( strcmp( error_type, "Unexpected_ERROR" ) == 0 )
      Print_error_unexpected( error_token );
    else
      Print_error_unexpected_2( error_token );
  } // Print_error_token()

};

class Error_tool {
  public:
  Token_tool mtool;
  int Atom_error_check( vector<Token>& token_list, int index ) {
    while ( strcmp( token_list[index].type, "endline" ) == 0 
            || strcmp( token_list[index].type, "COMMENT" ) == 0 ) {
      index++;
    } // while()

    // cout << index << " : " << token_list[index].data << endl << endl ;
    if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
      int head = index ;
      index = Paren_error_check( token_list, index+1 );
      if ( index )
        return index;
      else {
        while ( token_list[head].out_s_exp == false ) {
          if ( strcmp( token_list[head].type, "Unexpected_ERROR" ) != 0 )
            token_list.erase( token_list.begin() + head );
          else
            head++;
        } // while()

        return head;
      } // else
    } // if()
    else if ( strcmp( token_list[index].type, "DOT" ) == 0 ) {
      strcpy( token_list[index].type, "Unexpected_ERROR" );
      token_list[index].error_tag = true;
      return -1;
    } // else if() 
    else if ( strcmp( token_list[index].type, "RIGHT-PAREN" ) == 0 ) {
      strcpy( token_list[index].type, "Unexpected_ERROR" );
      token_list[index].error_tag = true;
      return -1;
    } // else if()
    else if ( strcmp( token_list[index].type, "STRING" ) == 0 )
      return String_error_check( token_list[index], token_list[index].data, index );
    else
      return index;
  } // Atom_error_check() 檢查單一atom是否合乎文法

  int Paren_error_check( vector<Token>& token_list, int index ) {
    bool have_dot = false ;
    bool have_s_exp = false;
    bool have_dot_exp = false;
    for ( ; strcmp( token_list[index].type, "RIGHT-PAREN" ) != 0 ; index++ ) {
      if ( strcmp( token_list[index].type, "DOT" ) == 0 ) {
        if ( !have_dot && have_s_exp ) {
          have_dot = true;
          index++;
          index = Atom_error_check( token_list, index );
          if ( index > 0 ) {
            have_dot_exp = true;
            int next = index+1 ;
            while ( strcmp( token_list[next].type, "endline" ) == 0 ) {
              next++;
            } // while()

            if ( strcmp( token_list[next].type, "RIGHT-PAREN" ) != 0 ) {
              strcpy( token_list[next].type, "Unexpected_ERROR_2" );
              token_list[next].error_tag = true;
              return -1;
            } // if()
            else
              return next;
          } // if()
          else
            return -1;
        } // if()
        else {
          if ( !have_dot && !have_s_exp ) 
            strcpy( token_list[index].type, "Unexpected_ERROR" );
          if ( have_dot )
            strcpy( token_list[index].type, "Unexpected_ERROR_2" );
          token_list[index].error_tag = true;
          return -1;
        } // else()
      } // if()
      else if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
        index = Paren_error_check( token_list, index+1 ) ;
        if ( index > 0 )
          have_s_exp = true;
        else
          return -1;
      } // else if()
      else if ( strcmp( token_list[index].type, "QUOTE" ) == 0 ) {
        index = Quote_error_check( token_list, index+1 ) ;
        if ( index > 0 )
          have_s_exp = true;
        else
          return -1;
      } // else if()
      else if ( strcmp( token_list[index].type, "STRING" ) == 0 ) {
        index = String_error_check( token_list[index], token_list[index].data, index ) ;
        if ( index > 0 )
          have_s_exp = true;
        else
          return -1;
      } // else if()
      else
        have_s_exp = true;
    } // for()

    return index ;
  } // Paren_error_check() 檢查括號內是否合乎文法

  int Quote_error_check( vector<Token>& token_list, int index ) {
    // cout << index << " : " << token_list[index].data << endl << endl ;
    if ( strcmp( token_list[index].type, "QUOTE" ) == 0 ) {
      index = Quote_error_check( token_list, index+1 ) ;
      if ( index > 0 )
        return index+1;
      else
        return -1;
    } // if()
    else if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
      index = Paren_error_check( token_list, index+1 );
      if ( index > 0 )
        return index;
      else
        return -1;
    } // else if()
    else {
      index = Atom_error_check( token_list, index ) ;
      if ( index > 0 )
        return index;
      else
        return -1;
    } // else()
  } // Quote_error_check()

  int String_error_check( Token& token_list, Str200 token_string, int index ) {
    int last = strlen( token_string ) - 1 ;
    
    if ( !token_list.is_string ) {
      strcpy( token_list.type, "no closing quote" );
      token_list.error_tag = true;
      return -1 ;
    } // if()
    else if ( token_list.is_string )
      return index;
    else {
      strcpy( token_list.type, "no closing quote" );
      token_list.error_tag = true;
      return -1 ;
    } // else()
  } // String_error_check()

  void Token_error_check( vector<Token>& token_list ) {
    for ( int index = 0 ; index < token_list.size() ; index++ ) {
      while ( strcmp( token_list[index].type, "endline" ) == 0 
              || strcmp( token_list[index].type, "COMMENT" ) == 0 ) {
        index++;
      } // while()

      token_list[index].error_tag = false;
      if ( strcmp( token_list[index].type, "QUOTE" ) == 0 ) {
        int head = index ;
        index = Quote_error_check( token_list, index+1 ) ;
        if ( index >= 0 )
          ;
        else {
          bool error_line = false;
          while ( strcmp( token_list[head].type, "endline" ) != 0 || !error_line ) {
            // cout << endl << endl << token_list[head].type << "  " << token_list[head].data;
            if ( strcmp( token_list[head].type, "endline" ) != 0 
                 && !token_list[head].error_tag )
              token_list.erase( token_list.begin() + head );
            else {
              if ( strcmp( token_list[head].type, "endline" ) != 0 )
                error_line = true;
              head++;
            } // else
          } // while()

          index = head;
        } // else
      } // if()
      else if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
        int head = index ;
        index = Paren_error_check( token_list, index+1 ) ;
        if ( index >= 0 )
          ;
        else {
          bool error_line = false;
          while ( strcmp( token_list[head].type, "endline" ) != 0 || !error_line ) {
            if ( strcmp( token_list[head].type, "endline" ) != 0 
                 && !token_list[head].error_tag )
              token_list.erase( token_list.begin() + head );
            else {
              if ( strcmp( token_list[head].type, "endline" ) != 0 )
                error_line = true;
              head++;
            } // else
          } // while()

          index = head;
        } // else
      } // else if()
      else {
        if ( Atom_error_check( token_list, index ) >= 0 )
          ;
        else {
          index++;
          while ( strcmp( token_list[index].type, "endline" ) != 0 ) {
            token_list.erase( token_list.begin() + index );
          } // while()

        } // else()
      } // else()
    } // for()
  } // Token_error_check()

};

class Tree_tool {
  public:
  vector<Tree_pointer> mtree_list; // 存取乾淨的tree;
  Token_tool mt_tool;
  Print_tool mp_tool;

  int Set_quote( vector<Token>& token_list, int& index, int paren ) {
    Token insert_token;
    mt_tool.Init_token( insert_token ) ;
    strcpy( insert_token.data, "(" );
    strcpy( insert_token.type, "LEFT-PAREN" ) ;
    token_list.insert( token_list.begin() + index, insert_token ) ;
    index = index + 2;
    if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
      paren++;
      index++;
    } // if()

    if ( strcmp( token_list[index].data, "\'" ) == 0 ) {
      index = Set_quote( token_list, index, 0 );
      if ( paren > 0 )
        index++;
    } // if()

    // for ( int j = 0 ; j < token_list.size() ; j++ ){
    //     cout << token_list[j].data << " ";
    // } // for
    // cout << index << " peek :" << token_list[index].data;
    // cout << endl << endl ;
      
    for ( ; paren > 0 ; index++ ) {
      if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
        paren++;
      } // if()
      else if ( strcmp( token_list[index].data, "\'" ) == 0 ) {
        index = Set_quote( token_list, index, 0 );
      } // else if()
      else if ( strcmp( token_list[index].type, "RIGHT-PAREN" ) == 0 ) {
        paren--;
        if ( paren == 0 )
          index--;
      } // else if()
      else
        ;

    } // for()
    
    mt_tool.Init_token( insert_token ) ;
    strcpy( insert_token.data, ")" );
    strcpy( insert_token.type, "RIGHT-PAREN" ) ;
    if ( index == token_list.size() - 1 ) {
      token_list.push_back( insert_token );
    } // if()
    else {
      token_list.insert( token_list.begin() + index + 1, insert_token ) ;
    } // else()

    // for ( int j = 0 ; j < token_list.size() ; j++ ){
    //     cout << token_list[j].data << " ";
    // } // for
    // cout << index << " after peek :" << token_list[index].data;
    // cout << endl << endl ;
    return index + 1 ;
  } // Set_quote() 建樹前先處理好quote

  int Set_dot( vector<Token>& token_list, int& index, bool insert_paren, int& paren ) {
    Token insert_token ;
    int middle_insert = 0;
    for ( ; strcmp( token_list[index].type, "RIGHT-PAREN" ) != 0 ; index++ ) {
      // for ( int j = 0 ; j < token_list.size() ; j++ ){
      //   cout << token_list[j].data << " ";
      // }
      // cout << index << " peek :" << token_list[index].data;
      // cout << endl << endl ;
      if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
        index++;
        paren++;
        index = Set_dot( token_list, index, false, paren ) ;
        paren--;
        if ( strcmp( token_list[index+1].data, "." ) != 0 ) {
          index++;
          mt_tool.Init_token( insert_token ) ;
          strcpy( insert_token.data, "." );
          strcpy( insert_token.type, "DOT" ) ;
          token_list.insert( token_list.begin() + index, insert_token ) ;
          index++;
          mt_tool.Init_token( insert_token ) ;
          strcpy( insert_token.data, "(" );
          strcpy( insert_token.type, "LEFT-PAREN" ) ;
          token_list.insert( token_list.begin() + index, insert_token ) ;
          middle_insert++;
        } // if()
        else {
          index = index + 2;
          if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
            index++;
            index = Set_dot( token_list, index, false, paren ) ;
          } // if()

          while ( strcmp( token_list[index+1].type, "RIGHT-PAREN" ) != 0 ) {
            index++;
          } // while()

        } // else()
          
      } // if()
      else {
        if ( strcmp( token_list[index+1].data, "." ) != 0 ) {
          index++;
          mt_tool.Init_token( insert_token ) ;
          strcpy( insert_token.data, "." );
          strcpy( insert_token.type, "DOT" ) ;
          token_list.insert( token_list.begin() + index, insert_token ) ;
          index++;
          mt_tool.Init_token( insert_token ) ;
          strcpy( insert_token.data, "(" );
          strcpy( insert_token.type, "LEFT-PAREN" ) ;
          token_list.insert( token_list.begin() + index, insert_token ) ;
          index++;
          index = Set_dot( token_list, index, true, paren ) ;
          while ( strcmp( token_list[index+1].type, "RIGHT-PAREN" ) != 0 ) {
            index++;
          } // while()
        } // if()
        else {
          index = index + 2;
          if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
            index++;
            index = Set_dot( token_list, index, false, paren ) ;
          } // if()

          int after_dot_paren = 0;
          while ( strcmp( token_list[index+1].type, "RIGHT-PAREN" ) != 0 
                  && after_dot_paren == 0 ) {
            if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 )
              after_dot_paren++;
            if ( strcmp( token_list[index].type, "RIGHT-PAREN" ) == 0 )
              after_dot_paren--;
            index++;
          } // while()

        } // else()
      } // else() 遇到非LEFT-PAREN
    } // for()

    if ( insert_paren ) {
      mt_tool.Init_token( insert_token ) ;
      strcpy( insert_token.data, ")" );
      strcpy( insert_token.type, "RIGHT-PAREN" ) ;
      token_list.insert( token_list.begin() + index, insert_token ) ;
    } // if()

    while ( middle_insert > 0 ) {
      mt_tool.Init_token( insert_token ) ;
      strcpy( insert_token.data, ")" );
      strcpy( insert_token.type, "RIGHT-PAREN" ) ;
      token_list.insert( token_list.begin() + index, insert_token ) ;
      index++;
      middle_insert--;
    } // while()
    // for ( int j = 0 ; j < token_list.size() ; j++ ){
    //   cout << token_list[j].data << " " ;
    // }
    // cout << index << " after peek :" << token_list[index].data;
    // cout << endl << endl ;

    return index;
  } // Set_dot() 建樹前準備，用dot來區分左右子樹

  int Build_tree( vector<Token>& token_list, Tree_pointer tree_token, int& index ) {
    tree_token->left = NULL;
    tree_token->right = NULL;
    bool arguement = false;
    for ( ; strcmp( token_list[index].type, "RIGHT-PAREN" ) != 0 ; index++ ) {
      if ( !arguement ) {
        if ( strcmp( token_list[index].type, "DOT" ) == 0 ) {
          arguement = true;
          if ( strcmp( token_list[index+1].type, "LEFT-PAREN" ) == 0 ) {
            index = index + 2 ;
            tree_token->right = new Tree_node;
            mt_tool.Init_token( tree_token->right->root ) ;
            index = Build_tree( token_list, tree_token->right, index ) ;
          } // if()，dot後面放left-paren
          else if ( strcmp( token_list[index+1].data, "nil" ) != 0 ) {
            tree_token->right = new Tree_node;
            mt_tool.Init_token( tree_token->right->root ) ;
            tree_token->right->root = token_list[index];
          } // else if()
        } // if()，遇到dot
        else if ( strcmp( token_list[index].type, "LEFT-PAREN" ) == 0 ) {
          tree_token->left = new Tree_node;
          mt_tool.Init_token( tree_token->left->root ) ;
          // tree_token->left->root = token_list[index];
          index++;
          index = Build_tree( token_list, tree_token->left, index );
        } // else if()
        else {
          tree_token->left = new Tree_node;
          tree_token->left->left = NULL;
          tree_token->left->right = NULL;
          tree_token->left->root = token_list[index];
          tree_token->left->root.produce_flag = false;
        } // else()
      } // if()
      else {
        tree_token->right = new Tree_node;
        tree_token->right->left = NULL;
        tree_token->right->right = NULL ;
        tree_token->right->root = token_list[index];
        tree_token->left->root.produce_flag = false;
      } // else()
    } // for()

    return index;
  } // Build_tree()

  void Sweep_tree( Tree_pointer& tree_token ) {
    if ( tree_token->right != NULL ) {
      if ( strcmp( tree_token->right->root.data, "nil" ) == 0 ) {
        delete tree_token->right ;
        tree_token->right = NULL;
      } // if()
    } // if()

    if ( strcmp( tree_token->root.data, "." ) == 0 ) {
      if ( strcmp( tree_token->left->root.data, "nil" ) == 0 ) {
        delete tree_token->left ;
        tree_token->left = NULL;
        mt_tool.Init_token( tree_token->root );
      } // if()
      else if ( strcmp( tree_token->left->root.data, "(" ) == 0 ) {
        mt_tool.Init_token( tree_token->root );
        mt_tool.Init_token( tree_token->left->root );
      } // else if()
    } // if()
    
    if ( tree_token->left != NULL )
      Sweep_tree( tree_token->left );
    if ( tree_token->right != NULL )
      Sweep_tree( tree_token->right );
  } // Sweep_tree()

  void Print_node( Tree_pointer tree_token ) {
    
    if ( strcmp( tree_token->root.type, "FLOAT" ) == 0 ) {
      cout << "> " << fixed <<  setprecision( 3 ) << tree_token->root.float_data << endl << endl ;
    } // if()
    else if ( strcmp( tree_token->root.type, "INT" ) == 0 ) {
      cout << "> " << tree_token->root.int_data << endl << endl ;
    } // else if()
    else if ( strcmp( tree_token->root.type, "LEFT-PAREN" ) == 0 ) {
      cout << ">" ;
      // p_tool.Print_s_exp( tool.mtoken_list, index, 1, 1 );
      cout << endl << endl ;
    } // else if() 要顯示s-exp
    else if ( strcmp( tree_token->root.type, "QUOTE" ) == 0 ) {
      cout << ">" ;
      // p_tool.Print_quote_s_exp( tool.mtoken_list, index, 1, 1 );
      cout << endl << endl ;
    } // else if()
    else {
      if ( mp_tool.Check_need_translate( tree_token->root.data ) ) {
        cout << "> " ;
        mp_tool.Translate_print( tree_token->root.data ) ;
        cout << endl << endl ;
      } // if() token內有特殊字元要轉譯
      else
        cout << "> " << tree_token->root.data << endl << endl ;
    } // else() token type不是int跟float

    cout << "> " << "ERROR (no more input) : END-OF-FILE encountered" 
         << endl << "Thanks for using OurScheme!" ;
      
  } // Print_node()

  void Print_tree( Tree_pointer tree_token, int& space, bool from_left, bool& error_tree ) {
    bool print_paren = false;
    if ( strcmp( tree_token->root.data, "" ) != 0
         || ( strcmp( tree_token->root.data, "" ) == 0 
              && strcmp( tree_token->root.type, "STRING" ) == 0 ) ) {
      if ( !from_left ) {
        for ( int counter = 0 ; counter < space ; counter++ ) {
          cout << " ";
        } // for()
      } // if()

      if ( strcmp( tree_token->root.type, "FLOAT" ) == 0 ) {
        cout << fixed << setprecision( 3 ) << tree_token->root.float_data << endl ;
      } // if()
      else if ( strcmp( tree_token->root.type, "INT" ) == 0 ) {
        cout << tree_token->root.int_data << endl;
      } // else if()
      else if ( strcmp( tree_token->root.type, "QUOTE" ) == 0 ) {
        cout << "quote" << endl;
      } // else if()
      else {
        if ( mp_tool.Check_need_translate( tree_token->root.data ) ) {
          if ( strcmp( tree_token->root.type, "STRING" ) == 0 ) {
            cout << '\"' ;
            mp_tool.Translate_print( tree_token->root.data ) ;
            cout << '\"' << endl ;
          } // if()
          else {
            mp_tool.Translate_print( tree_token->root.data ) ;
            cout << endl ;
          } // else()

        } // if() token內有特殊字元要轉譯
        else {
          if ( strcmp( tree_token->root.type, "STRING" ) == 0 ) {
            cout << '\"' ;
            cout << tree_token->root.data ;
            cout << '\"' << endl ;
          } // if()
          else {
            if ( strcmp( tree_token->root.type, "SYMBOL" ) == 0 
                 && tree_token->root.produce_flag && !error_tree )
              cout << "#<procedure " << tree_token->root.data << ">" << endl;
            else
              cout << tree_token->root.data << endl ;
          } // else()
        } // else()
      } // else() token type不是int跟float
    } // if() 該節點有值
    else {
      if ( !from_left ) {
        for ( int counter = 0 ; counter < space ; counter++ ) {
          cout << " ";
        } // for()
      } // if()
      else {
        cout << "( ";
        space = space + 2;
        print_paren = true;
      } // else()
    } // else() 該節點沒有值

    if ( tree_token->left != NULL )
      Print_tree( tree_token->left, space, true, error_tree ) ;
    if ( tree_token->right != NULL && strcmp( tree_token->right->root.type, "NIL" ) != 0 ) {
      if ( strcmp( tree_token->right->root.data, "" ) != 0 ) {
        for ( int counter = 0 ; counter < space ; counter++ ) {
          cout << " ";
        } // for()

        cout << "." << endl ;
      } // if() 右子樹的root有值

      Print_tree( tree_token->right, space, false, error_tree );
    } // if()

    if ( print_paren ) {
      space = space - 2;
      for ( int counter = 0 ; counter < space ; counter++ ) {
        cout << " ";
      } // for()

      cout << ")" << endl ;
    } // if()
  } // Print_tree()

  void Print_tree_inorder( Tree_pointer tree_token ) {
    if ( tree_token->left != NULL )
      Print_tree_inorder( tree_token->left ) ;
    if ( strcmp( tree_token->root.data, "" ) != 0 )
      cout << tree_token->root.data << " " ;
    if ( tree_token->right != NULL )
      Print_tree_inorder( tree_token->right );
    if ( strcmp( tree_token->root.data, "(" ) == 0 )
      cout << " " << ")" ;
  } // Print_tree_inorder()

  bool Check_feature( Str200 token_data ) {
    if ( strcmp( token_data, "cons" ) == 0 
         || strcmp( token_data, "list" ) == 0
         || strcmp( token_data, "quote" ) == 0 
         || strcmp( token_data, "\'" ) == 0
         || strcmp( token_data, "define" ) == 0 
         || strcmp( token_data, "car" ) == 0
         || strcmp( token_data, "cdr" ) == 0
         || strcmp( token_data, "atom?" ) == 0
         || strcmp( token_data, "pair?" ) == 0
         || strcmp( token_data, "list?" ) == 0
         || strcmp( token_data, "null?" ) == 0
         || strcmp( token_data, "integer?" ) == 0
         || strcmp( token_data, "real?" ) == 0
         || strcmp( token_data, "number?" ) == 0
         || strcmp( token_data, "string?" ) == 0
         || strcmp( token_data, "boolean?" ) == 0
         || strcmp( token_data, "symbol?" ) == 0
         || strcmp( token_data, "+" ) == 0
         || strcmp( token_data, "-" ) == 0
         || strcmp( token_data, "*" ) == 0
         || strcmp( token_data, "/" ) == 0
         || strcmp( token_data, "not" ) == 0
         || strcmp( token_data, "and" ) == 0
         || strcmp( token_data, "or" ) == 0
         || strcmp( token_data, ">" ) == 0
         || strcmp( token_data, ">=" ) == 0
         || strcmp( token_data, "<" ) == 0
         || strcmp( token_data, "<=" ) == 0
         || strcmp( token_data, "=" ) == 0
         || strcmp( token_data, "string-append" ) == 0
         || strcmp( token_data, "string>?" ) == 0
         || strcmp( token_data, "string<?" ) == 0
         || strcmp( token_data, "string=?" ) == 0
         || strcmp( token_data, "eqv?" ) == 0
         || strcmp( token_data, "equal?" ) == 0
         || strcmp( token_data, "begin" ) == 0
         || strcmp( token_data, "if" ) == 0
         || strcmp( token_data, "cond" ) == 0
         || strcmp( token_data, "clean-environment" ) == 0
         || strcmp( token_data, "exit" ) == 0
         || strcmp( token_data, "let" ) == 0
         || strcmp( token_data, "lambda" ) == 0 )
      return true;
    else
      return false;
  } // Check_feature()

  bool Check_feature_2( Str200 token_data ) {
    if ( strcmp( token_data, "cons" ) == 0 
         || strcmp( token_data, "list" ) == 0
         || strcmp( token_data, "quote" ) == 0 
         || strcmp( token_data, "\'" ) == 0
         || strcmp( token_data, "car" ) == 0
         || strcmp( token_data, "cdr" ) == 0
         || strcmp( token_data, "atom?" ) == 0
         || strcmp( token_data, "pair?" ) == 0
         || strcmp( token_data, "list?" ) == 0
         || strcmp( token_data, "null?" ) == 0
         || strcmp( token_data, "integer?" ) == 0
         || strcmp( token_data, "real?" ) == 0
         || strcmp( token_data, "number?" ) == 0
         || strcmp( token_data, "string?" ) == 0
         || strcmp( token_data, "boolean?" ) == 0
         || strcmp( token_data, "symbol?" ) == 0
         || strcmp( token_data, "+" ) == 0
         || strcmp( token_data, "-" ) == 0
         || strcmp( token_data, "*" ) == 0
         || strcmp( token_data, "/" ) == 0
         || strcmp( token_data, "and" ) == 0
         || strcmp( token_data, ">" ) == 0
         || strcmp( token_data, ">=" ) == 0
         || strcmp( token_data, "<" ) == 0
         || strcmp( token_data, "<=" ) == 0
         || strcmp( token_data, "=" ) == 0
         || strcmp( token_data, "string-append" ) == 0
         || strcmp( token_data, "string>?" ) == 0
         || strcmp( token_data, "string<?" ) == 0
         || strcmp( token_data, "string=?" ) == 0
         || strcmp( token_data, "eqv?" ) == 0
         || strcmp( token_data, "equal?" ) == 0
         || strcmp( token_data, "begin" ) == 0
         || strcmp( token_data, "if" ) == 0
         || strcmp( token_data, "cond" ) == 0
         || strcmp( token_data, "clean-environment" ) == 0 
         || strcmp( token_data, "exit" ) == 0
         || strcmp( token_data, "let" ) == 0
         || strcmp( token_data, "lambda" ) == 0 )
      return true;
    else
      return false;
  } // Check_feature_2()
};

class Eval_tool {
  public:
  Tree_tool mtr_tool;
  Token_tool mt_tool;
  // vector<Tree_pointer> output_list;

  bool Check_non_list( Tree_pointer tree_root, Tree_pointer& error_tree ) {
    error_tree = tree_root;
    while ( tree_root->right != NULL ) {
      tree_root = tree_root->right ;
    } // while()

    if ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
      return false;
    } // if() 如果最右下不是nil
    else {
      error_tree = NULL;
      return true;
    } // else()
  } // Check_non_list()

  bool Check_number_of_arguement( int need_arguement_type, Tree_pointer tree_root, 
                                  Tree_pointer& error_tree ) {
    int arguement_counter = 0;
    Tree_pointer counter_tree = new Tree_node;
    counter_tree = tree_root;
    while ( strcmp( counter_tree->root.data, "nil" ) != 0 ) {
      arguement_counter++;
      counter_tree = counter_tree->right;
    } // while()

    if ( need_arguement_type == 0 ) {
      if ( arguement_counter != 0 ) {
        error_tree = tree_root;
        return false;
      } // if()

      return true;
    } // if() type為0 (arguement == 0)
    else if ( need_arguement_type == 1 ) {
      if ( arguement_counter != 1 ) {
        error_tree = tree_root;
        return false;
      } // if()

      return true;
    } // else if() type為1 (arguement == 1)
    else if ( need_arguement_type == 2 ) {
      if ( arguement_counter != 2 ) {
        error_tree = tree_root;
        return false;
      } // if()

      return true;
    } // else if() type為2 (arguement == 2)
    else if ( need_arguement_type == 3 ) {
      if ( arguement_counter < 0 ) {
        error_tree = tree_root;
        return false;
      } // if()

      return true;
    } // else if() type為3 (arguement >= 0)
    else if ( need_arguement_type == 4 ) {
      if ( arguement_counter < 1 ) {
        error_tree = tree_root;
        return false;
      } // if()

      return true;
    } // else if() type為4 (arguement >= 1)
    else if ( need_arguement_type == 5 ) {
      if ( arguement_counter < 2 ) {
        error_tree = tree_root;
        return false;
      } // if()

      return true;
    } // else if() type為5 (arguement >= 2)
    else {
      if ( arguement_counter != 2 && arguement_counter != 3 ) {
        error_tree = tree_root;
        return false;
      } // if()

      return true;
    } // else if() type為6 (arguement == 2 or 3)
  } // Check_number_of_arguement()

  bool Check_type_of_arguement( Str200 function_name, Tree_pointer tree_root, Tree_pointer& error_tree ) {
    if ( strcmp( function_name, "+" ) == 0 
         || strcmp( function_name, "-" ) == 0
         || strcmp( function_name, "*" ) == 0
         || strcmp( function_name, "/" ) == 0
         || strcmp( function_name, ">" ) == 0
         || strcmp( function_name, ">=" ) == 0
         || strcmp( function_name, "=" ) == 0
         || strcmp( function_name, "<=" ) == 0
         || strcmp( function_name, "<" ) == 0 ) {
      while ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
        if ( strcmp( tree_root->left->root.type, "INT" ) != 0
             && strcmp( tree_root->left->root.type, "FLOAT" ) != 0 ) {
          error_tree = tree_root->left;
          return false;
        } // if() 如果是數字

        tree_root = tree_root->right;
      } // while()

      return true;
    } // if() function為四則運算
    else if ( strcmp( function_name, "string-append" ) == 0 
              || strcmp( function_name, "string>?" ) == 0
              || strcmp( function_name, "string<?" ) == 0
              || strcmp( function_name, "string=?" ) == 0 ) {
      while ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
        if ( strcmp( tree_root->left->root.type, "STRING" ) != 0 ) {
          error_tree = tree_root->left;
          return false;
        } // if() 如果是數字

        tree_root = tree_root->right;
      } // while()

      return true;
    } // else if() function為字串運算
    else if ( strcmp( function_name, "car" ) == 0
              || strcmp( function_name, "cdr" ) == 0 ) {
      if ( strcmp( tree_root->right->left->root.data, "" ) != 0 ) {
        error_tree = tree_root->right->left;
        return false;
      } // if()
      else {
        return true;
      } // else()
    } // else if()

    return true;
  } // Check_type_of_arguement()

  bool Check_attempt_non_function( Tree_pointer tree_root, Tree_pointer& error_tree ) {
    if ( strcmp( tree_root->root.type, "INT" ) == 0
         || strcmp( tree_root->root.type, "FLOAT" ) == 0
         || strcmp( tree_root->root.type, "STRING" ) == 0
         || strcmp( tree_root->root.type, "NIL" ) == 0
         || strcmp( tree_root->root.type, "T" ) == 0
         || strcmp( tree_root->root.data, "()" ) == 0 ) {
      error_tree = tree_root;
      return false;
    } // if() 如果不是原有function

    return true;
  } // Check_attempt_non_function()

  bool Check_is_internal_function( Tree_pointer& tree_root,
                                   vector<Binding>& global_arguement, vector<Binding>& local_arguement ) {
    if ( mtr_tool.Check_feature( tree_root->root.data ) 
         || tree_root->root.is_keep )
      return true;
    else {
      int function_index = 0 ;

      function_index = Find_binding( tree_root->root.data, local_arguement );
      if ( function_index != -1 ) {
        Tree_pointer change_tree = new Tree_node;
        change_tree = local_arguement[function_index].meaning;
        if ( mtr_tool.Check_feature( change_tree->root.data ) ) {
          tree_root = local_arguement[function_index].meaning;
          return true;
        } // if()
        else
          return false;
      } // if()

      function_index = Find_binding( tree_root->root.data, global_arguement );
      if ( function_index != -1 ) {
        Tree_pointer change_tree = new Tree_node;
        change_tree = global_arguement[function_index].meaning;
        if ( mtr_tool.Check_feature( change_tree->root.data ) ) {
          tree_root = global_arguement[function_index].meaning;
          return true;
        } // if()
        else
          return false;
      } // if()

      
      

      return false;
    } // else()
  } // Check_is_internal_function()

  bool Check_unbound_symbol( Tree_pointer& tree_root, Tree_pointer& error_tree,
                             vector<Binding>& global_arguement, vector<Binding>& local_arguement ) {
    if ( mtr_tool.Check_feature( tree_root->root.data ) 
         || tree_root->root.is_keep )
      return true;
    else {
      int function_index = 0 ;

      function_index = Find_binding( tree_root->root.data, local_arguement );
      if ( function_index != -1 ) {
        if ( !local_arguement[function_index].is_function ) {
          Tree_pointer change_tree = new Tree_node;
          change_tree = local_arguement[function_index].meaning;
          if ( mtr_tool.Check_feature( change_tree->root.data ) )
            tree_root = local_arguement[function_index].meaning;
        } // if()

        return true;
      } // if()

      function_index = Find_binding( tree_root->root.data, global_arguement );
      if ( function_index != -1 ) {
        if ( !global_arguement[function_index].is_function ) {
          Tree_pointer change_tree = new Tree_node;
          change_tree = global_arguement[function_index].meaning;
          if ( mtr_tool.Check_feature( change_tree->root.data ) )
            tree_root = global_arguement[function_index].meaning;
        } // if()

        return true;
      } // if()

      
      
      error_tree = tree_root;
      return false;
    } // else()
  } // Check_unbound_symbol()

  bool Check_format( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->left->root.data, "define" ) == 0 ) {
      if ( strcmp( tree_root->right->root.data, "nil" ) == 0 ) {
        cout << "ERROR (DEFINE format) : " ;
        return false;
      } // if()
      else if ( strcmp( tree_root->right->right->root.data, "nil" ) == 0 ) {
        cout << "ERROR (DEFINE format) : " ;
        return false;
      } // if() 沒有meaning
      else if ( strcmp( tree_root->right->right->right->root.data, "nil" ) != 0 ) {
        cout << "ERROR (DEFINE format) : " ;
        return false;
      } // if() define的參數數量不對
      else {
        Tree_pointer arguement_tree = new Tree_node;
        Tree_pointer meaning_tree = new Tree_node;
        arguement_tree = tree_root->right->left;
        meaning_tree = tree_root->right->right->left;
        if ( strcmp( arguement_tree->root.type, "SYMBOL" ) == 0 ) {
          if ( mtr_tool.Check_feature( arguement_tree->root.data ) ) {
            cout << "ERROR (DEFINE format) : " ;
            return false;
          } // if() 不能 redefined

          return true;
        } // if()
        else {
          while ( strcmp( arguement_tree->root.type, "NIL" ) != 0 ) {
            if ( strcmp( arguement_tree->left->root.type, "SYMBOL" ) != 0 ) {
              cout << "ERROR (DEFINE format) : " ;
              return false;
            } // if()

            arguement_tree = arguement_tree->right;
          } // while()

          if ( strcmp( meaning_tree->root.type, "NIL" ) == 0 ) {
            cout << "ERROR (DEFINE format) : " ;
            return false;
          } // if()

          return true;
        } // else()

        cout << "ERROR (DEFINE format) : " ;
        return false;
      } // else() 檢查要define的SYMBOL

    } // if()
    else if ( strcmp( tree_root->left->root.data, "set!" ) == 0 ) {
      return true; // 待補
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "let" ) == 0 ) {
      if ( strcmp( tree_root->right->root.data, "nil" ) == 0 ) {
        cout << "ERROR (LET format) : " ;
        return false;
      } // if()
      else if ( strcmp( tree_root->right->right->root.data, "nil" ) == 0 ) {
        cout << "ERROR (LET format) : " ;
        return false;
      } // else if()
      else {
        Tree_pointer arguement_tree = new Tree_node;
        arguement_tree = tree_root->right->left ;
        if ( strcmp( arguement_tree->root.data, "" ) != 0 ) {
          Tree_pointer check_arguement_tree = new Tree_node;
          while ( strcmp( arguement_tree->root.type, "NIL" ) != 0 ) {
            check_arguement_tree = arguement_tree->left;
            if ( strcmp( check_arguement_tree->root.data, "" ) != 0 ) {
              cout << "ERROR (LET format) : " ;
              return false;
            } // if()
            else {
              if ( strcmp( check_arguement_tree->left->root.type, "SYMBOL" ) != 0 ) {
                cout << "ERROR (LET format) : " ;
                return false;
              } // if()
              else if ( strcmp( check_arguement_tree->right->root.type, "NIL" ) == 0 ) {
                cout << "ERROR (LET format) : " ;
                return false;
              } // else if()
              else if ( strcmp( check_arguement_tree->right->right->root.type, "NIL" ) != 0 ) {
                cout << "ERROR (LET format) : " ;
                return false;
              } // else if()
            } // else

            arguement_tree = arguement_tree->right;
          } // while()
        } // if()

        return true;
      } // else
      
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "cond" ) == 0 ) {
      Tree_pointer check_cond_tree = new Tree_node;
      Tree_pointer error_tree = new Tree_node;
      check_cond_tree = tree_root->right;
      if ( strcmp( check_cond_tree->root.data, "nil" ) == 0 ) {
        cout << "ERROR (COND format) : " ;
        return false;
      } // if()

      while ( strcmp( check_cond_tree->root.data, "nil" ) != 0 ) {
        if ( !Check_non_list( check_cond_tree->left, error_tree ) ) {
          cout << "ERROR (COND format) : " ;
          return false;
        } // if()
        
        if ( !Check_number_of_arguement( 5, check_cond_tree->left, error_tree ) ) {
          cout << "ERROR (COND format) : " ;
          return false;
        } // if()

        check_cond_tree = check_cond_tree->right;
      } // while()
      
      return true;
    } // else if()
    else {
      Tree_pointer function_arguement_tree = new Tree_node;
      function_arguement_tree = tree_root->right->left;
      if ( strcmp( function_arguement_tree->root.data, "" ) != 0 ) {
        if ( strcmp( function_arguement_tree->root.type, "NIL" ) != 0 ) {
          cout << "ERROR (LAMBDA format) : " ;
          return false;
        } // if() 確認function_arguement_tree的atom不是NIL
      } // if() 
      else {
        while ( strcmp( function_arguement_tree->root.type, "NIL" ) != 0 ) {
          if ( strcmp( function_arguement_tree->left->root.type, "SYMBOL" ) != 0 ) {
            cout << "ERROR (LAMBDA format) : " ;
            return false;
          } // if()

          function_arguement_tree = function_arguement_tree->right;
        } // while()
      } // else()

      return true;
    } // else() lambda format
  } // Check_format()

  int Sort_arguement_num_type( Str200 function_name ) {
    if ( strcmp( function_name, "clean-environment" ) == 0
         || strcmp( function_name, "exit" ) == 0 )
      return 0;
    else if ( strcmp( function_name, "quote" ) == 0
              || strcmp( function_name, "\'" ) == 0
              || strcmp( function_name, "car" ) == 0
              || strcmp( function_name, "cdr" ) == 0
              || strcmp( function_name, "atom?" ) == 0
              || strcmp( function_name, "pair?" ) == 0
              || strcmp( function_name, "list?" ) == 0
              || strcmp( function_name, "null?" ) == 0
              || strcmp( function_name, "integer?" ) == 0
              || strcmp( function_name, "real?" ) == 0
              || strcmp( function_name, "number?" ) == 0
              || strcmp( function_name, "string?" ) == 0
              || strcmp( function_name, "boolean?" ) == 0
              || strcmp( function_name, "symbol?" ) == 0
              || strcmp( function_name, "not" ) == 0 )
      return 1;
    else if ( strcmp( function_name, "cons" ) == 0
              || strcmp( function_name, "define" ) == 0
              || strcmp( function_name, "eqv?" ) == 0
              || strcmp( function_name, "equal?" ) == 0 )
      return 2;
    else if ( strcmp( function_name, "list" ) == 0 )
      return 3;
    else if ( strcmp( function_name, "begin" ) == 0
              || strcmp( function_name, "cond" ) == 0 )
      return 4;
    else if ( strcmp( function_name, "+" ) == 0
              || strcmp( function_name, "-" ) == 0
              || strcmp( function_name, "*" ) == 0
              || strcmp( function_name, "/" ) == 0
              || strcmp( function_name, "and" ) == 0
              || strcmp( function_name, "or" ) == 0
              || strcmp( function_name, ">" ) == 0
              || strcmp( function_name, ">=" ) == 0
              || strcmp( function_name, "<" ) == 0
              || strcmp( function_name, "<=" ) == 0
              || strcmp( function_name, "=" ) == 0
              || strcmp( function_name, "string-append" ) == 0
              || strcmp( function_name, "string>?" ) == 0
              || strcmp( function_name, "string<?" ) == 0
              || strcmp( function_name, "string=?" ) == 0
              || strcmp( function_name, "let" ) == 0
              || strcmp( function_name, "lambda" ) == 0 )
      return 5;
    else
      return 6;
  } // Sort_arguement_num_type()

  bool Eval_is_exit( Tree_pointer& tree_root ) {
    if ( tree_root->left != NULL && tree_root->right != NULL ) {
      if ( strcmp( tree_root->left->root.data, "exit" ) == 0
           && strcmp( tree_root->right->root.data, "nil" ) == 0 )
        return true;
      else 
        return false;
    } // if()

    return false;
  } // Eval_is_exit()

  bool Eval_cons( Tree_pointer& tree_root ) {
    tree_root->left = tree_root->right->left;
    tree_root->right = tree_root->right->right->left;
    return true;
  } // Eval_cons()

  bool Eval_list( Tree_pointer& tree_root ) {
    tree_root = tree_root->right;
    return true;
  } // Eval_list()

  bool Eval_quote( Tree_pointer& tree_root ) {
    tree_root = tree_root->right->left;
    tree_root->root.quote_flag = true;
    return true;
  } // Eval_quote()

  bool Eval_define( Tree_pointer& tree_root,
                    vector<Binding>& global_arguement, vector<Binding>& local_arguement ) {
    Binding add;
    Tree_pointer defined_meaning = new Tree_node;
    defined_meaning = tree_root->right->right->left;
    mt_tool.Clear_char_pointer( add.name );
    if ( strcmp( tree_root->right->left->root.type, "SYMBOL" ) == 0 ) {
      strcpy( add.name, tree_root->right->left->root.data );
      add.is_function = false;
      int i = Find_binding( add.name, global_arguement );
      if ( i >= 0 ) 
        global_arguement.erase( global_arguement.begin() + i ) ;

      if ( !Tree_check_error( defined_meaning, tree_root->right->right->left, false,
                              global_arguement, local_arguement ) ) {
        tree_root = defined_meaning;
        return false;
      } // if()
      else {
        if ( strcmp( defined_meaning->type, "FUNCTION" ) == 0 ) {
          Binding local;
          Tree_pointer arguement_tree = new Tree_node;
          arguement_tree = defined_meaning->right->left;
          add.is_function = true;
          add.meaning = defined_meaning;
          add.function_type = defined_meaning->left;
          add.function_body = defined_meaning->right->right->left;
          while ( strcmp( arguement_tree->root.type, "NIL" ) != 0 ) {
            strcpy( local.name, arguement_tree->left->root.data );
            add.function_arguement.push_back( local );
            arguement_tree = arguement_tree->right;
          } // while()

          global_arguement.push_back( add );
          mt_tool.Clear_char_pointer( tree_root->root.data );
          mt_tool.Clear_char_pointer( tree_root->root.type );
          strcpy( tree_root->root.data, add.name );
          strcat( tree_root->root.data, " defined"  );
          tree_root->left = NULL;
          tree_root->right = NULL;
          return true;
        } // if()
        else {
          int i = Find_binding( defined_meaning->root.data, global_arguement );
          if ( i >= 0 && global_arguement[i].is_function ) {
            add.function_body = global_arguement[i].function_body;
            add.function_type = global_arguement[i].function_type;
            add.is_function = global_arguement[i].is_function;
            add.function_arguement.assign( global_arguement[i].function_arguement.begin(),
                                           global_arguement[i].function_arguement.end() );
          } // if()
          else
            add.meaning = defined_meaning;
          global_arguement.push_back( add );
          mt_tool.Clear_char_pointer( tree_root->root.data );
          mt_tool.Clear_char_pointer( tree_root->root.type );
          strcpy( tree_root->root.data, add.name );
          strcat( tree_root->root.data, " defined"  );
          tree_root->left = NULL;
          tree_root->right = NULL;
          return true;
        } // else()
      } // else()
    } // if() define symbol binding
    else {
      Binding local;
      Tree_pointer arguement_tree = new Tree_node;
      arguement_tree = tree_root->right->left;
      arguement_tree->left->root.produce_flag = true;
      strcpy( add.name, arguement_tree->left->root.data );
      add.function_type = arguement_tree->left;
      arguement_tree = arguement_tree->right;
      while ( strcmp( arguement_tree->root.type, "NIL" ) != 0 ) {
        strcpy( local.name, arguement_tree->left->root.data );
        add.function_arguement.push_back( local );
        arguement_tree = arguement_tree->right;
      } // while() function local arguement

      add.function_body = tree_root->right->right->left;
      int i = Find_binding( add.name, global_arguement );
      if ( i >= 0 ) 
        global_arguement.erase( global_arguement.begin() + i ) ;
      add.is_function = true;
      global_arguement.push_back( add );
      mt_tool.Clear_char_pointer( tree_root->root.data );
      mt_tool.Clear_char_pointer( tree_root->root.type );
      strcpy( tree_root->root.data, add.name );
      strcat( tree_root->root.data, " defined"  );
      tree_root->left = NULL;
      tree_root->right = NULL;
      return true;
    } // else() define function binding
  } // Eval_define()

  bool Eval_local_define( Tree_pointer& tree_root, vector<Binding>& global_arguement,
                          vector<Binding>& local_arguement ) {
    Binding add;
    Tree_pointer defined_meaning = new Tree_node;
    mt_tool.Clear_char_pointer( add.name );
    if ( !Tree_check_error( defined_meaning, tree_root->right->left, false,
                            global_arguement, local_arguement ) ) {
      tree_root = defined_meaning;
      return false;
    } // if()
    else {
      strcpy( add.name, tree_root->left->root.data );
      add.meaning = defined_meaning;
      int i = Find_binding( add.name, local_arguement );
      if ( i >= 0 ) 
        local_arguement.erase( local_arguement.begin() + i ) ;
      add.is_function = false;
      local_arguement.push_back( add );
      return true;
    } // else()
  } // Eval_local_define() 定義區域變數

  bool Eval_car( Tree_pointer& tree_root ) {
    Tree_pointer error_tree = new Tree_node;
    Str200 function_name;
    strcpy( function_name, "car" );
    if ( !Check_type_of_arguement( function_name, tree_root, error_tree ) ) {
      tree_root = error_tree;
      cout << "ERROR (car with incorrect argument type) : " ;
      return false;
    } // if()
    else {
      tree_root = tree_root->right->left->left;
      return true;
    } // else()
  } // Eval_car()

  bool Eval_cdr( Tree_pointer& tree_root ) {
    Tree_pointer error_tree = new Tree_node;
    Str200 function_name;
    strcpy( function_name, "cdr" );
    if ( !Check_type_of_arguement( function_name, tree_root, error_tree ) ) {
      tree_root = error_tree;
      cout << "ERROR (cdr with incorrect argument type) : " ;
      return false;
    } // if()
    else {
      tree_root = tree_root->right->left->right;
      return true;
    } // else()
  } // Eval_cdr()

  bool Eval_clean( Tree_pointer& tree_root, vector<Binding>& global_arguement ) {
    global_arguement.clear();
    tree_root = tree_root->left;
    strcpy( tree_root->root.data, "environment cleaned" );
    tree_root->root.produce_flag = false;
    return true;
  } // Eval_clean()

  bool Eval_is_atom( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "INT" ) == 0
         || strcmp( tree_root->root.type, "FLOAT" ) == 0
         || strcmp( tree_root->root.type, "STRING" ) == 0
         || strcmp( tree_root->root.type, "NIL" ) == 0
         || strcmp( tree_root->root.type, "T" ) == 0
         || strcmp( tree_root->root.data, "()" ) == 0 ) {
      return true;
    } // if() 如果是ATOM

    return false;
  } // Eval_is_atom()

  bool Eval_is_pair( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "INT" ) == 0
         || strcmp( tree_root->root.type, "FLOAT" ) == 0
         || strcmp( tree_root->root.type, "STRING" ) == 0
         || strcmp( tree_root->root.type, "NIL" ) == 0
         || strcmp( tree_root->root.type, "T" ) == 0
         || strcmp( tree_root->root.data, "()" ) == 0 ) {
      return false;
    } // if() 如果是ATOM

    return true;
  } // Eval_is_pair()

  bool Eval_is_list( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "INT" ) == 0
         || strcmp( tree_root->root.type, "FLOAT" ) == 0
         || strcmp( tree_root->root.type, "STRING" ) == 0
         || strcmp( tree_root->root.type, "NIL" ) == 0
         || strcmp( tree_root->root.type, "T" ) == 0
         || strcmp( tree_root->root.data, "()" ) == 0 ) {
      return false;
    } // if() 如果是ATOM
    else {
      while ( tree_root->right != NULL ) {
        tree_root = tree_root->right ;
      } // while()

      if ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
        return false;
      } // if() 如果最右下不是nil
      else
        return true;
    } // else()
  } // Eval_is_list()

  bool Eval_is_null( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "NIL" ) == 0
         || strcmp( tree_root->root.data, "()" ) == 0 ) {
      return true;
    } // if() 如果是nil

    return false;
  } // Eval_is_null()

  bool Eval_is_int( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "INT" ) == 0 ) {
      return true;
    } // if() 如果是int

    return false;
  } // Eval_is_int()

  bool Eval_is_real( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "INT" ) == 0
         || strcmp( tree_root->root.type, "FLOAT" ) == 0 ) {
      return true;
    } // if() 如果是real

    return false;
  } // Eval_is_real()

  bool Eval_is_number( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "INT" ) == 0
         || strcmp( tree_root->root.type, "FLOAT" ) == 0 ) {
      return true;
    } // if() 如果是number

    return false;
  } // Eval_is_number()

  bool Eval_is_string( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "STRING" ) == 0 ) {
      return true;
    } // if() 如果是number

    return false;
  } // Eval_is_string()

  bool Eval_is_boolean( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "NIL" ) == 0
         || strcmp( tree_root->root.type, "T" ) == 0 ) {
      return true;
    } // if() 如果是number

    return false;
  } // Eval_is_boolean()

  bool Eval_is_symbol( Tree_pointer tree_root ) {
    if ( strcmp( tree_root->root.type, "SYMBOL" ) == 0 ) {
      return true;
    } // if() 如果是number

    return false;
  } // Eval_is_symbol()

  bool Eval_plus( Tree_pointer& tree_root, float& sum, bool float_used ) {
    Str200 name;
    strcpy( name, "+" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (+ with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      tree_root = tree_root->right;
      if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) tree_root->left->root.int_data ;
      } // if()
      else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
        sum = tree_root->left->root.float_data ;
        float_used = true;
      } // else if()

      tree_root = tree_root->right;
      while ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
        if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
          sum = sum + ( float ) tree_root->left->root.int_data ;
        } // if()
        else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
          sum = sum + tree_root->left->root.float_data ;
          float_used = true;
        } // else if()

        tree_root = tree_root->right;
      } // while()

      tree_root->left = NULL;
      tree_root->right = NULL;
      if ( float_used ) {
        tree_root->root.float_data = sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "FLOAT" );
      } // if() 計算結果為浮點數
      else {
        tree_root->root.int_data = ( int ) sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "INT" );
      } // else() 計算結果為整數

      return true;
    } // else
  } // Eval_plus()

  bool Eval_minus( Tree_pointer& tree_root, float& sum, bool float_used ) {
    Str200 name;
    strcpy( name, "-" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (- with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      tree_root = tree_root->right;
      if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) tree_root->left->root.int_data ;
      } // if()
      else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
        sum = tree_root->left->root.float_data ;
        float_used = true;
      } // else if()

      tree_root = tree_root->right;
      while ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
        if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
          sum = sum - ( float ) tree_root->left->root.int_data ;
        } // if()
        else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
          sum = sum - tree_root->left->root.float_data ;
          float_used = true;
        } // else if()

        tree_root = tree_root->right;
      } // while()

      tree_root->left = NULL;
      tree_root->right = NULL;
      if ( float_used ) {
        tree_root->root.float_data = sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "FLOAT" );
      } // if() 計算結果為浮點數
      else {
        tree_root->root.int_data = ( int ) sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "INT" );
      } // else() 計算結果為整數

      return true;
    } // else
  } // Eval_minus()

  bool Eval_mul( Tree_pointer& tree_root, float& sum, bool float_used ) {
    Str200 name;
    strcpy( name, "*" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (* with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      tree_root = tree_root->right;
      if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) tree_root->left->root.int_data ;
      } // if()
      else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
        sum = tree_root->left->root.float_data ;
        float_used = true;
      } // else if()

      tree_root = tree_root->right;
      while ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
        if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
          sum = sum * ( float ) tree_root->left->root.int_data ;
        } // if()
        else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
          sum = sum * tree_root->left->root.float_data ;
          float_used = true;
        } // else if()

        tree_root = tree_root->right;
      } // while()

      tree_root->left = NULL;
      tree_root->right = NULL;
      if ( float_used ) {
        tree_root->root.float_data = sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "FLOAT" );
      } // if() 計算結果為浮點數
      else {
        tree_root->root.int_data = ( int ) sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "INT" );
      } // else() 計算結果為整數

      return true;
    } // else
  } // Eval_mul()

  bool Eval_div( Tree_pointer& tree_root, float& sum, bool float_used ) {
    Str200 name;
    strcpy( name, "/" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (/ with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      Tree_pointer symbol_tree = new Tree_node;
      symbol_tree = tree_root->left;
      tree_root = tree_root->right;
      if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) tree_root->left->root.int_data ;
      } // if()
      else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
        sum = tree_root->left->root.float_data ;
        float_used = true;
      } // else if()

      tree_root = tree_root->right;
      while ( strcmp( tree_root->root.data, "nil" ) != 0 ) {
        if ( strcmp( tree_root->left->root.type, "INT" ) == 0 ) {
          if ( tree_root->left->root.int_data != 0 )
            sum = sum / ( float ) tree_root->left->root.int_data ;
          else {
            cout << "ERROR (division by zero) : ";
            tree_root = symbol_tree;
            tree_root->root.error_tree = true;
            return false;
          } // else()
        } // if()
        else if ( strcmp( tree_root->left->root.type, "FLOAT" ) == 0 ) {
          if ( tree_root->left->root.float_data != 0.0 ) {
            sum = sum / tree_root->left->root.float_data ;
            float_used = true;
          } // if()
          else {
            cout << "ERROR (division by zero) : ";
            tree_root = symbol_tree;
            return false;
          } // else()
        } // else if()

        tree_root = tree_root->right;
      } // while()

      tree_root->left = NULL;
      tree_root->right = NULL;
      if ( float_used ) {
        tree_root->root.float_data = sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "FLOAT" );
      } // if() 計算結果為浮點數
      else {
        tree_root->root.int_data = ( int ) sum;
        mt_tool.Clear_char_pointer( tree_root->root.type );
        strcpy( tree_root->root.type, "INT" );
      } // else() 計算結果為整數

      return true;
    } // else
  } // Eval_div()

  bool Eval_not( Tree_pointer& tree_root ) {
    if ( strcmp( tree_root->right->left->root.data, "nil" ) == 0 ) {
      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
    } // if()
    else {
      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "nil" );
      strcpy( tree_root->root.type, "NIL" );
    } // else()

    return true;
  } // Eval_not()

  bool Eval_and( Tree_pointer& tree_root, vector<Binding>& global_arguement,
                 vector<Binding>& local_arguement ) {
    Tree_pointer keep_tree = new Tree_node;
    keep_tree = tree_root;
    keep_tree = keep_tree->right;
    while ( strcmp( keep_tree->root.data, "nil" ) != 0 ) {
      if ( !Tree_check_error( keep_tree->left, keep_tree->left, false,
                              global_arguement, local_arguement ) ) {
        tree_root = keep_tree->left;
        return false;
      } // if()

      if ( strcmp( keep_tree->left->root.data, "nil" ) == 0 ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
        return true;
      } // if()

      tree_root = keep_tree->left;
      keep_tree = keep_tree->right;
    } // while()

    return true;
  } // Eval_and()

  bool Eval_or( Tree_pointer& tree_root, vector<Binding>& global_arguement,
                vector<Binding>& local_arguement ) {
    Tree_pointer keep_tree = new Tree_node;
    keep_tree = tree_root;
    keep_tree = keep_tree->right;
    while ( strcmp( keep_tree->root.data, "nil" ) != 0 ) {
      if ( !Tree_check_error( keep_tree->left, keep_tree->left, false,
                              global_arguement, local_arguement ) ) {
        tree_root = keep_tree->left;
        return false;
      } // if()

      if ( strcmp( keep_tree->left->root.data, "nil" ) != 0 ) {
        tree_root = keep_tree->left;
        return true;
      } // if()

      keep_tree = keep_tree->right;
    } // while()

    mt_tool.Clear_char_pointer( tree_root->root.data ) ;
    mt_tool.Clear_char_pointer( tree_root->root.type ) ;
    tree_root->left = NULL;
    tree_root->right = NULL;
    strcpy( tree_root->root.data, "nil" );
    strcpy( tree_root->root.type, "NIL" );
    return true;
  } // Eval_or()

  bool Eval_bigger( Tree_pointer& tree_root ) {
    float sum = 0.0;
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, ">" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (> with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) calculate_tree->left->root.int_data ;
      } // if()
      else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
        sum = calculate_tree->left->root.float_data ;
      } // else if()

      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
          if ( sum <= ( float ) calculate_tree->left->root.int_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = ( float ) calculate_tree->left->root.int_data;
        } // if()
        else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
          if ( sum <= calculate_tree->left->root.float_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = calculate_tree->left->root.float_data;
        } // else if()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_bigger()

  bool Eval_no_smaller( Tree_pointer& tree_root ) {
    float sum = 0.0;
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, ">=" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (>= with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) calculate_tree->left->root.int_data ;
      } // if()
      else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
        sum = calculate_tree->left->root.float_data ;
      } // else if()

      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
          if ( sum < ( float ) calculate_tree->left->root.int_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = ( float ) calculate_tree->left->root.int_data;
        } // if()
        else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
          if ( sum < calculate_tree->left->root.float_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = calculate_tree->left->root.float_data;
        } // else if()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_no_smaller()

  bool Eval_smaller( Tree_pointer& tree_root ) {
    float sum = 0.0;
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, "<" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (< with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) calculate_tree->left->root.int_data ;
      } // if()
      else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
        sum = calculate_tree->left->root.float_data ;
      } // else if()

      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
          if ( sum >= ( float ) calculate_tree->left->root.int_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = ( float ) calculate_tree->left->root.int_data;
        } // if()
        else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
          if ( sum >= calculate_tree->left->root.float_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = calculate_tree->left->root.float_data;
        } // else if()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_smaller()

  bool Eval_no_bigger( Tree_pointer& tree_root ) {
    float sum = 0.0;
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, "<=" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (<= with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) calculate_tree->left->root.int_data ;
      } // if()
      else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
        sum = calculate_tree->left->root.float_data ;
      } // else if()

      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
          if ( sum > ( float ) calculate_tree->left->root.int_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = ( float ) calculate_tree->left->root.int_data;
        } // if()
        else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
          if ( sum > calculate_tree->left->root.float_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = calculate_tree->left->root.float_data;
        } // else if()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_no_bigger()

  bool Eval_num_equal( Tree_pointer& tree_root ) {
    float sum = 0.0;
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, "=" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (= with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
        sum = ( float ) calculate_tree->left->root.int_data ;
      } // if()
      else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
        sum = calculate_tree->left->root.float_data ;
      } // else if()

      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( calculate_tree->left->root.type, "INT" ) == 0 ) {
          if ( sum != ( float ) calculate_tree->left->root.int_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = ( float ) calculate_tree->left->root.int_data;
        } // if()
        else if ( strcmp( calculate_tree->left->root.type, "FLOAT" ) == 0 ) {
          if ( sum != calculate_tree->left->root.float_data ) {
            mt_tool.Clear_char_pointer( tree_root->root.data ) ;
            mt_tool.Clear_char_pointer( tree_root->root.type ) ;
            tree_root->left = NULL;
            tree_root->right = NULL;
            strcpy( tree_root->root.data, "nil" );
            strcpy( tree_root->root.type, "NIL" );
            return true;
          } // if()
          else
            sum = calculate_tree->left->root.float_data;
        } // else if()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_num_equal()

  bool Eval_string_append( Tree_pointer& tree_root ) {
    Str200 output_string;
    mt_tool.Clear_char_pointer( output_string );
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, "string-append" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (string-append with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        strcat( output_string, calculate_tree->left->root.data );
        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, output_string );
      strcpy( tree_root->root.type, "STRING" );
      return true;
    } // else
  } // Eval_string_append()

  bool Eval_is_string_bigger( Tree_pointer& tree_root ) {
    Str200 output_string;
    mt_tool.Clear_char_pointer( output_string );
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, "string>?" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (string>? with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      strcpy( output_string, calculate_tree->left->root.data );
      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( output_string, calculate_tree->left->root.data ) <= 0 ) {
          mt_tool.Clear_char_pointer( tree_root->root.data ) ;
          mt_tool.Clear_char_pointer( tree_root->root.type ) ;
          tree_root->left = NULL;
          tree_root->right = NULL;
          strcpy( tree_root->root.data, "nil" );
          strcpy( tree_root->root.type, "NIL" );
          return true;
        } // if()
        else {
          mt_tool.Clear_char_pointer( output_string );
          strcpy( output_string, calculate_tree->left->root.data );
        } // else()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_is_string_bigger()

  bool Eval_is_string_smaller( Tree_pointer& tree_root ) {
    Str200 output_string;
    mt_tool.Clear_char_pointer( output_string );
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, "string<?" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (string<? with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      strcpy( output_string, calculate_tree->left->root.data );
      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( output_string, calculate_tree->left->root.data ) >= 0 ) {
          mt_tool.Clear_char_pointer( tree_root->root.data ) ;
          mt_tool.Clear_char_pointer( tree_root->root.type ) ;
          tree_root->left = NULL;
          tree_root->right = NULL;
          strcpy( tree_root->root.data, "nil" );
          strcpy( tree_root->root.type, "NIL" );
          return true;
        } // if()
        else {
          mt_tool.Clear_char_pointer( output_string );
          strcpy( output_string, calculate_tree->left->root.data );
        } // else()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_is_string_smaller()

  bool Eval_is_string_equal( Tree_pointer& tree_root ) {
    Str200 output_string;
    mt_tool.Clear_char_pointer( output_string );
    Tree_pointer calculate_tree = new Tree_node;
    calculate_tree = tree_root;
    Str200 name;
    strcpy( name, "string=?" );
    if ( !Check_type_of_arguement( name, tree_root->right, tree_root->right ) ) {
      cout << "ERROR (string=? with incorrect argument type) : ";
      tree_root = tree_root->right;
      return false;
    } // if()
    else {
      calculate_tree = calculate_tree->right;
      strcpy( output_string, calculate_tree->left->root.data );
      calculate_tree = calculate_tree->right;
      while ( strcmp( calculate_tree->root.data, "nil" ) != 0 ) {
        if ( strcmp( output_string, calculate_tree->left->root.data ) != 0 ) {
          mt_tool.Clear_char_pointer( tree_root->root.data ) ;
          mt_tool.Clear_char_pointer( tree_root->root.type ) ;
          tree_root->left = NULL;
          tree_root->right = NULL;
          strcpy( tree_root->root.data, "nil" );
          strcpy( tree_root->root.type, "NIL" );
          return true;
        } // if()
        else {
          mt_tool.Clear_char_pointer( output_string );
          strcpy( output_string, calculate_tree->left->root.data );
        } // else()

        calculate_tree = calculate_tree->right;
      } // while()

      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
      return true;
    } // else
  } // Eval_is_string_equal()

  bool Eval_is_tree_eqv( Tree_pointer& tree_root ) {
    Tree_pointer tree1 = new Tree_node;
    Tree_pointer tree2 = new Tree_node;
    tree1 = tree_root->right->left;
    tree2 = tree_root->right->right->left;
    bool is_equal = false;
    if ( strcmp( tree1->root.data, "" ) != 0 && strcmp( tree2->root.data, "" ) != 0 
         && strcmp( tree1->root.type, "STRING" ) != 0 
         && strcmp( tree2->root.type, "STRING" ) != 0 ) {
      if ( strcmp( tree1->root.type, tree2->root.type ) == 0 ) {
        if ( strcmp( tree1->root.type, "INT" ) == 0 ) {
          if ( tree1->root.int_data == tree2->root.int_data )
            is_equal = true;
          else
            is_equal = false;
        } // if()
        else if ( strcmp( tree1->root.type, "FLOAT" ) == 0 ) {
          if ( tree1->root.float_data == tree2->root.float_data )
            is_equal = true;
          else
            is_equal = false;
        } // else if()
        else {
          if ( strcmp( tree1->root.data, tree2->root.data ) == 0 )
            is_equal = true;
          else
            is_equal = false;
        } // else()

      } // if()
      else if ( ( strcmp( tree1->root.type, "INT" ) == 0 
                  && strcmp( tree2->root.type, "FLOAT" ) == 0 ) 
                || ( strcmp( tree2->root.type, "INT" ) == 0 
                     && strcmp( tree1->root.type, "FLOAT" ) == 0 )  ) {
        if ( strcmp( tree1->root.type, "INT" ) == 0 ) {
          if ( tree1->root.int_data == tree2->root.float_data )
            is_equal = true;
          else
            is_equal = false;
        } // if()
        else if ( strcmp( tree1->root.type, "FLOAT" ) == 0 ) {
          if ( tree1->root.float_data == tree2->root.int_data )
            is_equal = true;
          else
            is_equal = false;
        } // else if()
      } // else if()

    } // if() 兩者都是atom
    else if ( tree1 == tree2 ) {
      is_equal = true;
    } // else if() 兩個都是list
    else {
      is_equal = false;
    } // else() 一個為atom，另一個為list

    if ( is_equal ) {
      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "#t" );
      strcpy( tree_root->root.type, "T" );
    } // if()
    else {
      mt_tool.Clear_char_pointer( tree_root->root.data ) ;
      mt_tool.Clear_char_pointer( tree_root->root.type ) ;
      tree_root->left = NULL;
      tree_root->right = NULL;
      strcpy( tree_root->root.data, "nil" );
      strcpy( tree_root->root.type, "NIL" );
    } // else()

    return true;
  } // Eval_is_tree_eqv()

  bool Eval_is_tree_equal( Tree_pointer tree1, Tree_pointer tree2 ) {
    if ( tree1->left != NULL && tree2->left != NULL ) {
      if ( !Eval_is_tree_equal( tree1->left, tree2->left ) )
        return false;
    } // if() 兩棵樹都還沒到葉，皆有左子樹
    else if ( tree1->left == NULL && tree2->left == NULL )
      ;
    else
      return false;

    if ( tree1->right != NULL && tree2->right != NULL ) {
      if ( !Eval_is_tree_equal( tree1->right, tree2->right ) )
        return false;
    } // if() 兩棵樹都還沒到葉，皆有右子樹
    else if ( tree1->right == NULL && tree2->right == NULL )
      ;
    else
      return false;

    if ( strcmp( tree1->root.data, tree2->root.data ) == 0 )
      return true;
    else
      return false;  
  } // Eval_is_tree_equal()

  bool Eval_begin( Tree_pointer& tree_root ) {
    Tree_pointer evaling_tree = new Tree_node;
    evaling_tree = tree_root->right;

    while ( strcmp( evaling_tree->root.data, "nil" ) != 0 ) {
      
      if ( strcmp( evaling_tree->right->root.data, "nil" ) == 0 ) {
        tree_root = evaling_tree->left;
        return true;
      } // if()

      evaling_tree = evaling_tree->right;
    } // while()

    return false;
  } // Eval_begin()

  bool Eval_if( Tree_pointer& tree_root, vector<Binding>& global_arguement,
                vector<Binding>& local_arguement ) {
    bool is_good = false;
    Tree_pointer check_tree = new Tree_node;
    Copy_tree( check_tree, tree_root->right->left );
    if ( !Tree_check_error( check_tree, check_tree, false, global_arguement, local_arguement ) ) {
      tree_root = check_tree;
      return false;
    } // if()

    if ( strcmp( check_tree->root.data, "nil" ) != 0 )
      is_good = true;

    if ( is_good ) {
      if ( strcmp( tree_root->right->right->root.data, "nil" ) != 0 ) {
        tree_root = tree_root->right->right->left;
        if ( !Tree_check_error( tree_root, tree_root, false, global_arguement, local_arguement ) )
          return false;
        else
          return true;
      } // if()
      else {
        return false ;
      } // else()
    } // if()
    else {
      if ( strcmp( tree_root->right->right->right->root.data, "nil" ) != 0 ) {
        tree_root = tree_root->right->right->right->left;
        if ( !Tree_check_error( tree_root, tree_root, false, global_arguement, local_arguement ) )
          return false;
        else 
          return true;
      } // if()
      else {
        cout << "ERROR (no return value) : ";
        return false ;
      } // else()
    } // else

  } // Eval_if()

  bool Eval_cond( Tree_pointer& tree_root, vector<Binding>& global_arguement,
                  vector<Binding>& local_arguement ) {
    Tree_pointer check_tree = new Tree_node;
    check_tree = tree_root;
    check_tree = check_tree->right;
    while ( strcmp( check_tree->root.data, "nil" ) != 0 ) {
      Tree_pointer inside_tree = new Tree_node;
      Tree_pointer test_tree = new Tree_node;
      inside_tree = check_tree->left;
      if ( strcmp( check_tree->right->root.data, "nil" ) == 0 ) {
        if ( strcmp( inside_tree->left->root.data, "else" ) == 0 )
          inside_tree->left->root.is_keep = true;
      } // if() 最後一個block

      Copy_tree( test_tree, inside_tree->left );

      if ( test_tree->root.is_keep 
           || Tree_check_error( test_tree, test_tree, false, global_arguement, local_arguement ) ) {
        if ( strcmp( test_tree->root.data, "nil" ) != 0 ) {
          inside_tree = inside_tree->right;
          while ( strcmp( inside_tree->root.data, "nil" ) != 0 ) {
            if ( !Tree_check_error( inside_tree->left, inside_tree->left, false,
                                    global_arguement, local_arguement ) ) {
              tree_root = inside_tree->left;
              return false;
            } // if()

            if ( strcmp( inside_tree->right->root.data, "nil" ) == 0 ) {
              tree_root = inside_tree->left;
              return true;
            } // if()
          
            inside_tree = inside_tree->right;
          } // while()

          return true;
        } // if()
      } // if()
      else {
        tree_root = test_tree;
        return false;
      } // else()

      check_tree = check_tree->right;
    } // while()

    cout << "ERROR (no return value) : " ;
    return false;
  } // Eval_cond()

  bool Eval_let( Tree_pointer& tree_root, vector<Binding>& global_arguement,
                 vector<Binding> local_arguement ) {
    Tree_pointer arguement_tree = new Tree_node;
    arguement_tree = tree_root->right->left;
    while ( strcmp( arguement_tree->root.type, "NIL" ) != 0 ) {
      if ( !Eval_local_define( arguement_tree->left, global_arguement, local_arguement ) ) {
        tree_root = arguement_tree->left;
        return false ;
      } // if()

      arguement_tree = arguement_tree->right;
    } // while()

    Tree_pointer return_eval_tree = new Tree_node;
    return_eval_tree = tree_root->right->right;
    while ( strcmp( return_eval_tree->root.type, "NIL" ) != 0 ) {
      if ( !Tree_check_error( return_eval_tree->left, return_eval_tree->left, false,
                              global_arguement, local_arguement ) ) {
        tree_root = return_eval_tree->left;
        return false;
      } // if()

      if ( strcmp( return_eval_tree->right->root.type, "NIL" ) == 0 ) {
        tree_root = return_eval_tree->left;
        return true;
      } // if()

      return_eval_tree = return_eval_tree->right;
    } // while()

    cout << "ERROR (no return value) : " ;
    return false;
  } // Eval_let()

  bool Eval_lambda( Tree_pointer& tree_root, vector<Binding>& global_arguement ) {
    tree_root->left->root.produce_flag = true;
    mt_tool.Clear_char_pointer( tree_root->type );
    strcpy( tree_root->type, "FUNCTION" ) ;

    return true;
  } // Eval_lambda()

  bool Input_local_arguement( vector<Binding>& global_arguement, vector<Binding>& local_arguement,
                              vector<Binding> function_arguement, Tree_pointer arguement_tree ) {
    Tree_pointer counter_tree = new Tree_node;
    counter_tree = arguement_tree;
    int arguement_num = 0;
    while ( strcmp( counter_tree->root.type, "NIL" ) != 0 ) {
      arguement_num++;
      counter_tree = counter_tree->right;
    } // while()

    if ( arguement_num != function_arguement.size() ) {
      cout << "ERROR (incorrect number of arguments) :" ;
      return false;
    } // if()
    else {
      int index = 0;

      vector<Binding> outside;
      outside.insert( outside.begin(), global_arguement.begin(), global_arguement.end() );
      outside.insert( outside.end(), local_arguement.begin(), local_arguement.end() );
      vector<Binding> local;
      for ( int i = 0 ; i < function_arguement.size() ; i++ ) {
        Tree_check_error( function_arguement[i].meaning, arguement_tree->left, false,
                          outside, local );
        index = Find_binding( function_arguement[i].meaning->root.data, outside );
        Binding add = function_arguement[i];
        if ( index >= 0 && outside[index].is_function ) {
          add.function_body = outside[index].function_body;
          add.function_type = outside[index].function_type;
          add.is_function = true ;
          add.function_arguement.assign( outside[index].function_arguement.begin(),
                                         outside[index].function_arguement.end() );
        } // if()
        else
          function_arguement[i].is_function = false;

        arguement_tree = arguement_tree->right;
        local_arguement.push_back( add );
      } // for()

      return true;
    } // else()
  } // Input_local_arguement()

  void Copy_tree( Tree_pointer& copy_tree, Tree_pointer original_tree ) {
    if ( original_tree->left != NULL ) {
      copy_tree->left = new Tree_node;
      Copy_tree( copy_tree->left, original_tree->left );
    } // if()
    else
      copy_tree->left = NULL;

    if ( original_tree->right != NULL ) {
      copy_tree->right = new Tree_node;
      Copy_tree( copy_tree->right, original_tree->right );
    } // if()
    else
      copy_tree->right = NULL;

    strcpy( copy_tree->root.data, original_tree->root.data );
    strcpy( copy_tree->root.type, original_tree->root.type );
    copy_tree->root.int_data = original_tree->root.int_data;
    copy_tree->root.float_data = original_tree->root.float_data;
    copy_tree->root.is_keep = original_tree->root.is_keep;

  } // Copy_tree()

  int Find_binding( Str200 root_name, vector<Binding>& arguement_list ) {
    for ( int i = arguement_list.size() - 1 ; i >= 0 ; i-- ) {
      if ( strcmp( root_name, arguement_list[i].name ) == 0 ) {
        return i;
      } // if()
    } // for()

    return -1;
  } // Find_binding()

  bool Eval_tree( Tree_pointer& tree_root, vector<Binding>& global_arguement,
                  vector<Binding>& local_arguement ) {
    if ( strcmp( tree_root->left->root.data, "cons" ) == 0 ) {
      Eval_cons( tree_root ) ;
    } // if()
    else if ( strcmp( tree_root->left->root.data, "list" ) == 0 ) {
      Eval_list( tree_root ) ;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "quote" ) == 0 
              || strcmp( tree_root->left->root.data, "\'" ) == 0 ) {
      Eval_quote( tree_root ) ;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "define" ) == 0 ) {
      if ( !Eval_define( tree_root, global_arguement, local_arguement ) )
        return false ;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "car" ) == 0 ) {
      if ( !Eval_car( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "cdr" ) == 0 ) {
      if ( !Eval_cdr( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "atom?" ) == 0 ) {
      if ( Eval_is_atom( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "pair?" ) == 0 ) {
      if ( Eval_is_pair( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "list?" ) == 0 ) {
      if ( Eval_is_list( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "null?" ) == 0 ) {
      if ( Eval_is_null( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "integer?" ) == 0 ) {
      if ( Eval_is_int( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "real?" ) == 0 ) {
      if ( Eval_is_real( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "number?" ) == 0 ) {
      if ( Eval_is_number( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "string?" ) == 0 ) {
      if ( Eval_is_string( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "boolean?" ) == 0 ) {
      if ( Eval_is_boolean( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "symbol?" ) == 0 ) {
      if ( Eval_is_symbol( tree_root->right->left ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "+" ) == 0 ) {
      float sum = 0.0;
      bool float_used = false;
      if ( !Eval_plus( tree_root, sum, float_used ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "-" ) == 0 ) {
      float sum = 0.0;
      bool float_used = false;
      if ( !Eval_minus( tree_root, sum, float_used ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "*" ) == 0 ) {
      float sum = 0.0;
      bool float_used = false;
      if ( !Eval_mul( tree_root, sum, float_used ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "/" ) == 0 ) {
      float sum = 0.0;
      bool float_used = false;
      if ( !Eval_div( tree_root, sum, float_used ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "not" ) == 0 ) {
      Eval_not( tree_root );
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "and" ) == 0 ) {
      if ( !Eval_and( tree_root, global_arguement, local_arguement ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "or" ) == 0 ) {
      if ( !Eval_or( tree_root, global_arguement, local_arguement ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, ">" ) == 0 ) {
      if ( !Eval_bigger( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, ">=" ) == 0 ) {
      if ( !Eval_no_smaller( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "<" ) == 0 ) {
      if ( !Eval_smaller( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "<=" ) == 0 ) {
      if ( !Eval_no_bigger( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "=" ) == 0 ) {
      if ( !Eval_num_equal( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "string-append" ) == 0 ) {
      if ( !Eval_string_append( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "string>?" ) == 0 ) {
      if ( !Eval_is_string_bigger( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "string<?" ) == 0 ) {
      if ( !Eval_is_string_smaller( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "string=?" ) == 0 ) {
      if ( !Eval_is_string_equal( tree_root ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "eqv?" ) == 0 ) {
      Eval_is_tree_eqv( tree_root );
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "equal?" ) == 0 ) {
      Tree_pointer tree1 = new Tree_node;
      Tree_pointer tree2 = new Tree_node;
      tree1 = tree_root->right->left;
      tree2 = tree_root->right->right->left;
      if ( Eval_is_tree_equal( tree1, tree2 ) ) {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "#t" );
        strcpy( tree_root->root.type, "T" );
      } // if()
      else {
        mt_tool.Clear_char_pointer( tree_root->root.data ) ;
        mt_tool.Clear_char_pointer( tree_root->root.type ) ;
        tree_root->left = NULL;
        tree_root->right = NULL;
        strcpy( tree_root->root.data, "nil" );
        strcpy( tree_root->root.type, "NIL" );
      } // else()
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "begin" ) == 0 ) {
      Eval_begin( tree_root );
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "if" ) == 0 ) {
      if ( !Eval_if( tree_root, global_arguement, local_arguement ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "cond" ) == 0 ) {
      if ( !Eval_cond( tree_root, global_arguement, local_arguement ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "clean-environment" ) == 0 ) {
      Eval_clean( tree_root, global_arguement ) ;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "let" ) == 0 ) {
      if ( !Eval_let( tree_root, global_arguement, local_arguement ) )
        return false;
    } // else if()
    else if ( strcmp( tree_root->left->root.data, "lambda" ) == 0 ) {
      if ( !Eval_lambda( tree_root, global_arguement ) )
        return false;
    } // else if()

    return true;
  } // Eval_tree()

  bool Tree_check_error( Tree_pointer& output_tree, Tree_pointer tree_root, bool top_level,
                         vector<Binding>& global_arguement, vector<Binding> local_arguement ) {
    Tree_pointer error_tree = new Tree_node;
    if ( Eval_is_atom( tree_root ) 
         && strcmp( tree_root->root.type, "SYMBOL" ) != 0 ) {
      output_tree = tree_root;
      return true;
    } // if() root為atom
    else if ( strcmp( tree_root->root.type, "SYMBOL" ) == 0 ) {
      if ( !Check_unbound_symbol( tree_root, error_tree, global_arguement, local_arguement ) ) {
        output_tree = error_tree;
        output_tree->root.error_tree = true;
        cout << "ERROR (unbound symbol) : ";
        return false;
      } // if()
      else {
        if ( mtr_tool.Check_feature( tree_root->root.data ) ) {
          if ( !tree_root->root.quote_flag )
            tree_root->root.produce_flag = true;
          output_tree = tree_root;
          return true;
        } // if() symbol為原有function
        else {
          int index = 0;
  
          index = Find_binding( tree_root->root.data, local_arguement );
          if ( index >= 0 && !local_arguement[index].is_function ) {
            tree_root = local_arguement[index].meaning;
            output_tree = tree_root;
            return true;
          } // if()
          else if ( index >= 0 && local_arguement[index].is_function ) {
            tree_root = local_arguement[index].function_type;
            output_tree = tree_root;
            return true;
          } // else if()

          index = Find_binding( tree_root->root.data, global_arguement );
          if ( index >= 0 && !global_arguement[index].is_function ) {
            tree_root = global_arguement[index].meaning;
            output_tree = tree_root;
            return true;
          } // if()
          else if ( index >= 0 && global_arguement[index].is_function ) {
            tree_root = global_arguement[index].function_type;
            output_tree = tree_root;
            return true;
          } // else if()

          return false;
        } // else()
      } // else()
    } // else if() root為symbol
    else {
      if ( !Check_non_list( tree_root, error_tree ) ) {
        output_tree = error_tree;
        cout << "ERROR (non-list) : ";
        return false;
      } // if() 如果list樹最右下不是nil
      else if ( !Check_attempt_non_function( tree_root->left, error_tree ) ) {
        output_tree = error_tree;
        cout << "ERROR (attempt to apply non-function) : ";
        output_tree->root.error_tree = true;
        return false;
      } // else if() 如果list第一個參數是atom
      else if ( strcmp( tree_root->left->root.type, "SYMBOL" ) == 0
                || strcmp( tree_root->left->root.type, "QUOTE" ) == 0 ) {
        if ( !Check_unbound_symbol( tree_root->left, error_tree, global_arguement, local_arguement ) ) {
          output_tree = error_tree;
          output_tree->root.error_tree = true;
          cout << "ERROR (unbound symbol) : ";
          return false;
        } // if()
        else {
          int index = 0;
          if ( tree_root->left != NULL )
            index = Find_binding( tree_root->left->root.data, local_arguement );
          if ( index >= 0 && !local_arguement[index].is_function ) {
            tree_root = local_arguement[index].meaning;
            index = -1;
          } // if()
          else if ( index >= 0 && local_arguement[index].is_function ) {
            Input_local_arguement( global_arguement, local_arguement,
                                   local_arguement[index].function_arguement, tree_root->right );
            Tree_pointer function_tree = new Tree_node;
            Copy_tree( function_tree, local_arguement[index].function_body );
            tree_root = function_tree;
          } // else if()

          if ( tree_root->left != NULL )
            index = Find_binding( tree_root->left->root.data, global_arguement );
          if ( index >= 0 && !global_arguement[index].is_function ) {
            tree_root = global_arguement[index].meaning;
            index = -1;
          } // if()
          else if ( index >= 0 && global_arguement[index].is_function ) {
            Input_local_arguement( global_arguement, local_arguement,
                                   global_arguement[index].function_arguement, tree_root->right );
            Tree_pointer function_tree = new Tree_node;
            Copy_tree( function_tree, global_arguement[index].function_body );
            tree_root = function_tree;
          } // else if()

          while ( index >= 0 ) {
            if ( tree_root->left != NULL )
              index = Find_binding( tree_root->left->root.data, local_arguement );
            if ( index >= 0 && !local_arguement[index].is_function ) {
              index = -1;
            } // if()
            else if ( index >= 0 && local_arguement[index].is_function ) {
              Input_local_arguement( global_arguement, local_arguement,
                                     local_arguement[index].function_arguement, tree_root->right );
              Tree_pointer function_tree = new Tree_node;
              Copy_tree( function_tree, local_arguement[index].function_body );
              tree_root = function_tree;
            } // else if()

            if ( tree_root->left != NULL )
              index = Find_binding( tree_root->left->root.data, global_arguement );
            if ( index >= 0 && !global_arguement[index].is_function ) {
              index = -1;
            } // if()
            else if ( index >= 0 && global_arguement[index].is_function ) {
              Input_local_arguement( global_arguement, local_arguement,
                                     global_arguement[index].function_arguement, tree_root->right );
              Tree_pointer function_tree = new Tree_node;
              Copy_tree( function_tree, global_arguement[index].function_body );
              tree_root = function_tree;
            } // else if()

          } // while ()
        } // else()

        if ( !Check_is_internal_function( tree_root->left, global_arguement, local_arguement ) ) {

          if ( !Check_attempt_non_function( tree_root, error_tree ) ) {
            output_tree = error_tree;
            cout << "ERROR (attempt to apply non-function) : ";
            output_tree->root.error_tree = true;
            return false;
          } // if() 如果list第一個參數是atom

        } // if() 如果list的第一個symbol不是已有的function
        else {
          
          int need_num_type = Sort_arguement_num_type( tree_root->left->root.data );

          if ( !top_level 
               && ( strcmp( tree_root->left->root.data, "clean-environment" ) == 0 
                    || strcmp( tree_root->left->root.data, "exit" ) == 0
                    || strcmp( tree_root->left->root.data, "define" ) == 0 ) ) {
            if ( strcmp( tree_root->left->root.data, "clean-environment" ) == 0 ) {
              mt_tool.Clear_char_pointer( tree_root->left->root.data );
              strcpy( tree_root->left->root.data, "ERROR (level of CLEAN-ENVIRONMENT)" );
            } // if()
            else if ( strcmp( tree_root->left->root.data, "exit" ) == 0 ) {
              mt_tool.Clear_char_pointer( tree_root->left->root.data );
              strcpy( tree_root->left->root.data, "ERROR (level of EXIT)" );
            } // else if()
            else if ( strcmp( tree_root->left->root.data, "define" ) == 0 ) {
              mt_tool.Clear_char_pointer( tree_root->left->root.data );
              strcpy( tree_root->left->root.data, "ERROR (level of DEFINE)" );
            } // else if()

            output_tree = tree_root->left;
            output_tree->left = NULL;
            output_tree->right = NULL;
            return false;
          } // if() 不在top-level

          else if ( strcmp( tree_root->left->root.data, "define" ) == 0
                    || strcmp( tree_root->left->root.data, "set!" ) == 0
                    || strcmp( tree_root->left->root.data, "let" ) == 0
                    || strcmp( tree_root->left->root.data, "cond" ) == 0
                    || strcmp( tree_root->left->root.data, "lambda" ) == 0 ) {
            if ( !Check_format( tree_root ) ) {
              output_tree = tree_root;
              return false;
            } // if()

            if ( Eval_tree( tree_root, global_arguement, local_arguement ) ) {
              output_tree = tree_root;
              return true;
            } // if()
            else {
              output_tree = tree_root;
              return false;
            } // else()

          } // else if()
          else if ( strcmp( tree_root->left->root.data, "if" ) == 0
                    || strcmp( tree_root->left->root.data, "and" ) == 0
                    || strcmp( tree_root->left->root.data, "or" ) == 0 ) {
            if ( !Check_number_of_arguement( need_num_type, tree_root->right, error_tree ) ) {
              output_tree = tree_root->left;
              cout << "ERROR (incorrect number of arguments) : ";
              return false;
            } // if()

            if ( Eval_tree( tree_root, global_arguement, local_arguement ) ) {
              output_tree = tree_root;
              return true;
            } // if()
            else {
              output_tree = tree_root;
              return false;
            } // else()

          } // else if()
          else {
            if ( !Check_number_of_arguement( need_num_type, tree_root->right, error_tree ) ) {
              cout << "ERROR (incorrect number of arguments) : ";
              output_tree = tree_root->left;
              output_tree->root.error_tree = true;
              return false;
            } // if()
          
          } // else()

        } // else()

        if ( strcmp( tree_root->left->root.type, "QUOTE" ) == 0 
             || strcmp( tree_root->left->root.data, "quote" ) == 0 ) {
          if ( Eval_tree( tree_root, global_arguement, local_arguement ) ) {
            output_tree = tree_root;
            return true;
          } // if()
          else {
            output_tree = tree_root;
            return false;
          } // else()

        } // if() 如果function是quote 
      } // else if() 如果第一個參數是SYMBOL
      else {
        int index = Find_binding( tree_root->left->left->root.data, global_arguement );
        if ( !Tree_check_error( tree_root->left, tree_root->left, false,
                                global_arguement, local_arguement ) ) {
          tree_root = tree_root->left;
          return false;
        } // if() 計算的第一個參數

        if ( strcmp( tree_root->left->type, "FUNCTION" ) != 0 ) {
          if ( !Check_is_internal_function( tree_root->left, global_arguement, local_arguement ) ) {
            output_tree = tree_root->left;
            cout << "ERROR (attempt to apply non-function) : ";
            return false;
          } // if() 如果第一個參數eval出來的結果不是function
          else {
            int need_num_type = Sort_arguement_num_type( tree_root->left->root.data );
            if ( !Check_number_of_arguement( need_num_type, tree_root->right, error_tree ) ) {
              cout << "ERROR (incorrect number of arguments) : ";
              output_tree = tree_root->left;
              output_tree->root.error_tree = true;
              return false;
            } // if()
          } // else()
        } // if() 如果第一個參數是function
        else {
          vector<Binding> function_arguement;
          Binding local;
          Tree_pointer function_tree = new Tree_node;
          function_tree = tree_root->left;
          Tree_pointer arguement_tree = new Tree_node;
          arguement_tree = function_tree->right->left;
          while ( strcmp( arguement_tree->root.type, "NIL" ) != 0 ) {
            strcpy( local.name, arguement_tree->left->root.data );
            function_arguement.push_back( local );
            arguement_tree = arguement_tree->right;
          } // while()

          Tree_pointer body_tree = new Tree_node;
          body_tree = function_tree->right->right;
          while ( strcmp( body_tree->right->root.type, "NIL" ) != 0 ) {
            body_tree = body_tree->right;
          } // while()
          
          Input_local_arguement( global_arguement, local_arguement,
                                 function_arguement, tree_root->right );
          
          tree_root = body_tree->left;
        } // else()
      } // else() 如果第一個參數是S-expression
      
      Tree_pointer calculator_tree = new Tree_node;
      calculator_tree = tree_root;
      Tree_pointer inside_tree = new Tree_node;
      while ( strcmp( calculator_tree->root.data, "nil" ) != 0 ) {
        
        if ( !Tree_check_error( inside_tree, calculator_tree->left, false,
                                global_arguement, local_arguement ) ) {
          output_tree = inside_tree;
          return false;
        } // if()
        else {
          calculator_tree->left = inside_tree;
          inside_tree = NULL;
        } // else()

        if ( strcmp( calculator_tree->left->root.data, "define" ) == 0 )
          calculator_tree = calculator_tree->right;
        calculator_tree = calculator_tree->right;
      } // while() 計算後面參數

      if ( Eval_tree( tree_root, global_arguement, local_arguement ) ) {
        mt_tool.Clear_char_pointer( tree_root->type );
        output_tree = tree_root;
        return true;
      } // if()
      else {
        output_tree = tree_root;
        return false;
      } // else()

    } // else() root為list
  } // Tree_check_error()
};

int main( void ) {
  Token_tool tool;
  Print_tool p_tool;
  Parser_tool pa_tool;
  Error_tool e_tool;
  Tree_tool tr_tool;
  Eval_tool ev_tool;
  vector<Binding> global_binding ;
  vector<Binding> local_binding ;
  int line = 1;
  bool eof_flag = true ;
  char ch = '\0' ;
  if ( 1 )
    scanf( "%d%c", &uTestNum, &ch ) ;
  cout << "Welcome to OurScheme!" << endl << endl ; 
  tool.Read_input( tool.minput_line, tool.mtoken_list, tool.mpeek_char ) ;

  for ( int index = 0 ; index < tool.mtoken_list.size() ; index++ ) {
    tool.Sort_token_type( tool.mtoken_list[index].data, tool.mtoken_list[index] );
  } // for() 分類token type

  pa_tool.Parser_token( tool.mtoken_list );
  e_tool.Token_error_check( tool.mtoken_list );
  tool.Set_line_number( tool.mtoken_list );
  tool.Set_column_number( tool.mtoken_list );
  tool.Clean_token_list( tool.mtoken_list );

  for ( int i = 0 ; i < tool.mtoken_list.size() ; i++ ) {
    if ( strcmp( tool.mtoken_list[i].data, "\'" ) == 0 ) {
      bool paren = false;
      i = tr_tool.Set_quote( tool.mtoken_list, i, paren );

      if ( strcmp( tool.mtoken_list[i].data, "\'" ) == 0 )
        i--;
    } // if()
  } // for()

  for ( int i = 0 ; i < tool.mtoken_list.size() ; i++ ) {
    if ( strcmp( tool.mtoken_list[i].type, "LEFT-PAREN" ) == 0 ) {
      int index = i+1;
      int paren = 1;
      i = tr_tool.Set_dot( tool.mtoken_list, index, false, paren );
    } // if()
  } // for()

  pa_tool.Parser_token( tool.mtoken_list );

  for ( int i = 0 ; i < tool.mtoken_list.size() ; i++ ) {
    Tree_pointer tree_token = new Tree_node;
    tree_token->root = tool.mtoken_list[i];
    tree_token->left = NULL;
    tree_token->right = NULL;
    if ( strcmp( tool.mtoken_list[i].type, "LEFT-PAREN" ) == 0 ) {
      i++;
      tool.Init_token( tree_token->root );
      i = tr_tool.Build_tree( tool.mtoken_list, tree_token, i );
    } // if()

    tr_tool.mtree_list.push_back( tree_token ) ;
  } // for()

  

  Tree_pointer output_tree = new Tree_node;
  for ( int i = 0 ; i < tr_tool.mtree_list.size() ; i++ ) {  
    cout << "> " ;
    if ( !ev_tool.Eval_is_exit( tr_tool.mtree_list[i] ) ) {
      if ( tr_tool.mtree_list[i]->root.error_tag ) {
        p_tool.Print_error_token( tr_tool.mtree_list[i]->root.type, tr_tool.mtree_list[i]->root );
      } // if()
      else {
        ev_tool.Tree_check_error( output_tree, tr_tool.mtree_list[i], true, global_binding, local_binding );
        int space = 0 ;
        if ( strcmp( output_tree->type, "FUNCTION" ) == 0 )
          tr_tool.Print_tree( output_tree->left, space, true, output_tree->left->root.error_tree );
        else
          tr_tool.Print_tree( output_tree, space, true, output_tree->root.error_tree );
      } // else()

      cout << endl ;
    } // if()
    else {
      eof_flag = false;
      i = i + tr_tool.mtree_list.size() ;
    } // else()

  } // for()

  // pa_tool.Parser_token( tool.mtoken_list );

  // for ( int index = 0 ; index < tool.mtoken_list.size() ; index++ ) {
  //  cout << tool.mtoken_list[index].type << " " << index << ": " 
  //       << tool.mtoken_list[index].data << " line : " 
  //       << tool.mtoken_list[index].line << " column : " 
  //       << tool.mtoken_list[index].column << " last : " << tool.mtoken_list[index].inputline_last << " "
  //       << "in : " << tool.mtoken_list[index].in_s_exp << "out : " << tool.mtoken_list[index].out_s_exp
  //       << " error : " << tool.mtoken_list[index].error_tag
  //       << endl << endl ;
  // } // for() 分類token type

  if ( eof_flag ) {
    cout << "> " << "ERROR (no more input) : END-OF-FILE encountered" 
         << endl << "Thanks for using OurScheme!" ;
  } // if() 遇到不是(exit)結尾情況
  else {
    cout << endl << "Thanks for using OurScheme!" ;
  } // else() 遇到是(exit)結尾情況 


  // for ( int index = 0 ; index < tool.minput_line.size() ; index++ ) {
  //  cout << index << ": " << tool.minput_line[index].data << endl << endl ;
  // } // for() 分類token type

  system("pause") ;
  return 0;
} // main()










