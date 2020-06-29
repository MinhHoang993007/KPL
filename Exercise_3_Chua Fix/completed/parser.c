/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;//make a new level 1 pointer point to the value which currentToken(level 1 pointer) are pointing to
  currentToken = lookAhead;//currentToken point to the value which lookAhead(level 1 pointer) are pointing to
  lookAhead = getValidToken();//point lookAhead to a new value
  free(tmp);//free(tmp) to free the memory of the value of currentToken point to in the pass that is needn't now
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {
  switch(lookAhead->tokenType){
  case TK_IDENT:
    compileConstDecl();
    compileConstDecls();
    break;
    //check FOLLOW to reach empty
  case KW_TYPE:
  case KW_VAR:
  case KW_FUNCTION:
  case KW_PROCEDURE:
  case KW_BEGIN:
    break;
  default:
    error(ERR_INVALIDCONSTDECL,lookAhead->lineNo,lookAhead->colNo);
    break;
  }
}

void compileConstDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {
  switch(lookAhead->tokenType){
  case TK_IDENT:
    compileTypeDecl();
    compileTypeDecls();
    break;
    //check FOLLOW to reach empty
  case KW_VAR:
  case KW_FUNCTION:
  case KW_PROCEDURE:
  case KW_BEGIN:
    break;
  default:
    error(ERR_INVALIDTYPEDECL,lookAhead->lineNo,lookAhead->colNo);
    break;
  }
}

void compileTypeDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) {
  switch(lookAhead->tokenType){
  case TK_IDENT:
    compileVarDecl();
    compileVarDecls();
    break;
    //check FOLLOW to reach empty
  case KW_FUNCTION:
  case KW_PROCEDURE:
  case KW_BEGIN:
    break;
  default:
    error(ERR_INVALIDVARDECL,lookAhead->lineNo,lookAhead->colNo);
    break;
  }
}

void compileVarDecl(void) {
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  switch(lookAhead->tokenType){
  case KW_FUNCTION:
    compileFuncDecl();
    compileSubDecls();
    break;
  case KW_PROCEDURE:
    compileProcDecl();
    compileSubDecls();
    break;
    //check FOLLOW to reach empty
  case KW_BEGIN:
    break;
  default:
    error(ERR_INVALIDSUBDECL,lookAhead->lineNo,lookAhead->colNo);
    break;
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
  switch(lookAhead->tokenType){
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  default:
    error(ERR_INVALIDCONSTANT,lookAhead->lineNo,lookAhead->colNo);
    break;
  }
}

void compileConstant(void) {
  switch(lookAhead->tokenType){
  case SB_PLUS:
    eat(SB_PLUS);
    compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileConstant2();
    break;
  case TK_IDENT:
  case TK_NUMBER:
    compileConstant2();
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  case SB_SEMICOLON:
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileConstant2(void) {
  switch(lookAhead->tokenType){
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileType(void) {
  switch(lookAhead->tokenType){
  case  KW_INTEGER:
    eat(KW_INTEGER);
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileType();
    break;
  default:
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileBasicType(void) {
  switch(lookAhead->tokenType){
  case KW_INTEGER:
    eat(KW_INTEGER);
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    break;
  default:
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileParams(void) {
  switch(lookAhead->tokenType){
  case SB_LPAR:
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
    break;
    //check FOLLOW to reach empty
  case SB_COLON:
  case SB_SEMICOLON:
    break;
  default:
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileParams2(void) {
  switch(lookAhead->tokenType){
  case SB_SEMICOLON:
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
    break;
    //check FOLLOW to reach empty
  case SB_RPAR:
    break;
  default:
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileParam(void) {
  switch(lookAhead->tokenType){
  case TK_IDENT:
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
    break;
  case KW_VAR:
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
    break;
  default:
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileStatements(void) {
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  switch(lookAhead->tokenType){
  case SB_SEMICOLON:
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
    break;
    //check FOLLOW to reach empty
  case KW_END:
    break;
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

//Exercise 3 ------------------------------------------------------
void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    
    //Exercise 3 ------------------------------------------------------
  case KW_SWITCH:
    compileSwitchSt();
    break;
  case KW_CASE:
    compileCaseSt();
    break;
  case KW_DEFAULT:
    compileDefaultSt();
    break;
  case KW_BREAK:
    compileBreakSt();
    break;
    //----------------------------------------------------------------------------

    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}


void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  eat(TK_IDENT);
  if (lookAhead->tokenType==SB_LSEL) compileIndexes();
  eat(SB_ASSIGN);
  compileExpression();
  //we don't have to check the FIRST() or FOLLOW() condition here
  //because compileStatement() have set it
  assert("Assign statement parsed ....");
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  //we don't have to check the FIRST() or FOLLOW() condition here
  //because compileStatement() have set it
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  //we don't have to check the FIRST() or FOLLOW() condition here
  //because compileStatement() have set it
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  //we don't have to check the FIRST() or FOLLOW() condition here
  //because compileStatement() have set it
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType==KW_ELSE) compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  switch(lookAhead->tokenType){
  case KW_ELSE:
    eat(KW_ELSE);
    compileStatement();
    break;
    //check FOLLOW to reach empty
  case SB_SEMICOLON:
  case KW_END:
    break;
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  //we don't have to check the FIRST() or FOLLOW() condition here
  //because compileStatement() have set it
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  //we don't have to check the FIRST() or FOLLOW() condition here
  //because compileStatement() have set it
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

//Exercise 3 --------------------------------------------------------------
void compileSwitchSt(void) {
  eat(KW_SWITCH);
  compileExpression();
  eat(KW_BEGIN);
  compileCaseSt();
  eat(KW_END);
}

void compileCaseSt(void) {
  eat(KW_CASE);
  compileConstant();
  eat(SB_COLON);
  compileStatements();
}

void compileDefaultSt(void) {
  eat(KW_DEFAULT);
  eat(SB_COLON);
  compileStatement();
}

void compileBreakSt(void) {
  eat(KW_BREAK);
}
//---------------------------------------------------------------------------------

void compileArguments(void) {
  switch(lookAhead->tokenType){
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
    break;
    //check FOLLOW to reach empty
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case SB_TIMES:
  case SB_SLASH:
  case KW_TO:
  case KW_DO:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RPAR:
  case SB_RSEL:
  case KW_THEN:
  		
    //danger!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  case SB_PLUS:
  case SB_MINUS:
    break;

  default:
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileArguments2(void) {
  switch(lookAhead->tokenType){
  case SB_COMMA:
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
    break;
    //check FOLLOW to reach empty
  case SB_RPAR:
    break;
  default:
    error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileCondition(void) {
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  switch(lookAhead->tokenType){
  case SB_EQ :
    eat(SB_EQ);
    compileExpression();
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    compileExpression();
    break;
  case SB_LE:
    eat(SB_LE);
    compileExpression();
    break;
  case SB_LT:
    eat(SB_LT);
    compileExpression();
    break;
  case SB_GE:
    eat(SB_GE);
    compileExpression();
    break;
  case SB_GT:
    eat(SB_GT);
    compileExpression();
    break;
  default:
    error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileExpression(void) {
  assert("Parsing an expression");
  switch(lookAhead->tokenType){
  case SB_PLUS:
    eat(SB_PLUS);
    compileExpression2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileExpression2();
    break;
  default:
    compileExpression2();
    break;
  }
  assert("Expression parsed");
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}

void compileExpression3(void) {
  switch(lookAhead->tokenType){
  case SB_PLUS:
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
    break;
    //check FOLLOW to reach empty
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_TO:
  case KW_DO:
    //EXERCISE 3 ----------------------------------------------------------------
    //Voi switch case se den begin.
    //VD: switch a
    //        begin
    //=> phai them ky hieu ket thuc vao fllow cuar expresstion
  case KW_BEGIN:
    //----------------------------------------------------------------------------------
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RPAR:
  case SB_RSEL:
  case KW_THEN:
    break;

  default:
    error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  switch(lookAhead->tokenType){
  case SB_MU:
    eat(SB_MU);
    compileFactor();
    compileTerm2();
    break;
  case SB_TIMES:
    eat(SB_TIMES);
    compileFactor();
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    compileFactor();
    compileTerm2();
    break;
    //check FOLLOW to reach empty
  case SB_PLUS:
  case SB_MINUS:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_TO:
  case KW_DO:
    //EXERCISE 3 ----------------------------------------------------------------------------------
  case KW_BEGIN:
    //----------------------------------------------------------------------------------------------------
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RPAR:
  case SB_RSEL:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileFactor(void) {
  switch(lookAhead->tokenType){
  case TK_CHAR:
  case TK_NUMBER:
    compileUnsignedConstant();
    break;
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    switch(lookAhead->tokenType){
    case SB_LSEL:
      compileIndexes();
      break;
    case SB_LPAR:
      compileArguments();
      break;
    default:
      break;
    }
    break;

  default:
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileIndexes(void) {
  switch(lookAhead->tokenType){
  case SB_LSEL:
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
    compileIndexes();
    break;
  default:
    break;
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
