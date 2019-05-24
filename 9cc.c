#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//トークンの型を表す値
enum{
  TK_NUM = 256, //整数トークン
  TK_EOF,       //入力の終わりを表すトークン
};

//トークンの型
//type
//value(tyがTK_NUMの場合、その数値)
//トークンの文字列
typedef struct {
  int ty;
  int val;
  char* input;
} Token;

char* user_input;

//トークナイズした結果のトークン列はこの配列に保存する
//100個以上のトークンは来ないものとする。
Token tokens[100];

//エラーを報告するための関数
//printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  vfprintf(stderr,fmt,ap);
  fprintf(stderr,"\n");
  va_end(ap);
  exit(1);
}

//エラー個所を報告する関数。
//locってなんやろ
void error_at(char *loc, char *msg)
{
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");//pos個数の空白を出力
  fprintf(stderr, "^ %s\n",msg);
  exit(1);
}

//user_inputが指している文字列を
//トークンに分割してtokensに保存する
void tokenize(){
  char* p = user_input;

  int i = 0;
  while (*p) {
    //空白文字をスキップ
    if(isspace(*p)){
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)){
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p,&p,10);//別にここでpは次のトークンか空白に移動しているからp++はいらない。
      i++;
      continue;
    }

    error_at(p,"トークナイズできません。");
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}
  
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  //トークナイズする
  user_input = argv[1];
  tokenize();

  //アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  //式の最初が数でないといけないので検査
  //最初のmov命令
  if(tokens[0].ty != TK_NUM)
    error_at(tokens[0].input,"数ではありません");
  printf("  mov rax, %d\n",tokens[0].val);

  // '+<数>'あるいは'-<数>'というトークンの並びを消費しつつ
  //アセンブリを出力

  int i = 1;
  while(tokens[i].ty != TK_EOF){
    if (tokens[i].ty == '+'){
      i++;
      if(tokens[i].ty != TK_NUM)
	error_at(tokens[i].input, "数ではありません");
      printf("  add rax, %d\n",tokens[i].val);
      i++;
    }
    else if (tokens[i].ty == '-'){
      i++;
      if(tokens[i].ty != TK_NUM)
	error_at(tokens[i].input,"数ではありません");
      printf("  sub rax, %d\n",tokens[i].val);
      i++;
    }
    else{
      error_at(tokens[i].input,"予期しないトークンです");
  }
  }
  
  printf("  ret\n");
  return 0;
}
