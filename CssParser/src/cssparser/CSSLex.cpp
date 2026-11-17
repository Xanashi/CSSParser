//
//  CSSLex.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "cssparser/CSSLex.h"
#include "utils/ContainerUtil.h"

#include <string.h>

#define NextChar(buffer) m_forwardPos >= m_bufferSize ? 0 : *(buffer + m_forwardPos++)
#define ErrorInLoop STATUS = LexError;stopLoop = true;
#define WS_CASE ' ': case '\r': case '\n': case '\t': case '\f'
#define NUMBER_CASE '0': case '1': case '2': case '3': case '4':\
case '5': case '6': case '7': case '8': case '9'

namespace future {
    Lex::Lex()
    {
        m_buffer = 0;
        m_firstPos = 0;
        m_forwardPos = 0;
        m_status = Start;
    }
    
    Lex::~Lex()
    {
        CleanResource();
    }
    
    void Lex::SetBufferSource(const std::string& fileName)
    {
        if (fileName.empty()) {
            return;
        }
        FILE* fileHandler;
        fopen_s(&fileHandler, fileName.c_str(), "r");
        if ( !fileHandler ) {
            return;
        }
        size_t bufferSize = 0;
        fseek(fileHandler, 0, SEEK_END);
        bufferSize = ftell(fileHandler);
        if ( !bufferSize ) {
            return;
        }
        fseek(fileHandler, 0, SEEK_SET);
        if (m_buffer) {
            delete [] m_buffer;
            m_buffer = 0;
        }
        m_buffer = new char[bufferSize];
        memset((void *)m_buffer, 0, bufferSize);
        m_bufferSize = fread((void *)m_buffer, 1, bufferSize, fileHandler);
        fclose(fileHandler);
        m_firstPos = 0;
        m_forwardPos = 0;
        m_status = Start;
    }
    
    void Lex::SetBufferString(const std::string &bufferString)
    {
        if (bufferString.empty()) {
            return;
        }
        if (m_buffer) {
            delete [] m_buffer;
            m_buffer = 0;
        }
        m_bufferSize = bufferString.size();
        m_buffer = new char[m_bufferSize];
        memcpy((void *)m_buffer, bufferString.data(), m_bufferSize);
        m_firstPos = 0;
        m_forwardPos = 0;
        m_status = Start;
    }

    Lex::CSSToken* Lex::GetTextToken(char stringType)
    {
        enum {
            _START = 0,
            _ESCAPESTART,
            _STRING
        };
        char status = _START;
        CSSToken* token = new CSSToken;
        m_tokenCache.insert(token);
        m_forwardPos = m_firstPos;
        if (m_forwardPos >= m_bufferSize) {
            token->type = END;
            return token;
        }
        char stringBoundary = stringType == 1 ? '"' : '\'';
        while (1) {
            char c = NextChar(m_buffer);
            switch (status) {
                case _START: {
                    if ((c != '\r' && c != '\n' && c != '\f' && c != stringBoundary) || (c & 0x80)) {
                        status = _STRING;
                        break;
                    } else if (c == '\\') {
                        status = _ESCAPESTART;
                        break;
                    }
                    break;
                }
                case _ESCAPESTART: {
                    if (isDigitalCharacter(c) || isLetter(c)) {
                        // [0-9a-z]{1,6}
                        for(int i = 0; i < 5; i ++) {
                            c = NextChar(m_buffer);
                            if (isDigitalCharacter(c) || isLetter(c)) {
                                continue;
                            } else {
                                break;
                            }
                        }
                    } else if (c == '\r' || c == '\n' || c == '\f') {
                        // \\{nl}
                        // nl \n|\r\n|\r|\f
                    } else {
                        // escape's \\[^\n\r\f0-9a-f]
                    }
                    status = _STRING;
                    break;
                }
                case _STRING: {
                    if ((c != '\r' && c != '\n' && c != '\f' && c != stringBoundary) || (c & 0x80) || c == '\\') {
                        --m_forwardPos;
                        status = _START;
                    } else {
                        token->type = STRING;
                        token->data = createData(m_firstPos, m_forwardPos);
                        return token;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        return token;
    }
    
    Lex::CSSToken* Lex::GetIdentToken()
    {
        CSSToken* token = new CSSToken;
        m_tokenCache.insert(token);
        token->type = IDENT;
        CSSDFAStatus STATUS = Start;
        bool stopLoop = false;
        while(1) {
            if (m_forwardPos >= m_bufferSize) {
                STATUS = end;
                break;
            }
            unsigned char c = NextChar(m_buffer);
            switch (STATUS) {
                case Start: {
                    if (c == IDENT_START_SIGN) {
                        STATUS = iDentStart;
                    } else if (isLetter(c) || c == UNDER_LINE_SIGN || (c & 0x80)) {
                        STATUS = NMStart;
                    } else if (c == BACK_SPLASH) {
                        STATUS = EscapeStartInNMStart;
                    } else if (isWs(c)) {
                        STATUS = Ws;
                    } else if (c == HASH_SIGN) {
                        STATUS = HashStart;
                    } else if (c == KEYWORD_SIGN) {
                        STATUS = AtKeyWordStart;
                    } else {
                        STATUS = LexError;
                        stopLoop = true;
                    }
                    break;
                }
                case iDentStart: {
                    if (isLetter(c) || c == UNDER_LINE_SIGN || (c & 0x80)) {
                        STATUS = NMStart;
                    } else if (c == BACK_SPLASH) {
                        STATUS = EscapeStartInNMStart;
                    } else {
                        STATUS = LexError;
                        stopLoop = true;
                    }
                    break;
                }
                case NMStart: {
                    if (isLetter(c) || isDigitalCharacter(c) || c == UNDER_LINE_SIGN || c == IDENT_START_SIGN || (c &0x80)) {
                        STATUS = NMChar;
                    } else if (c == BACK_SPLASH) {
                        STATUS = EscapeStartInNMChar;
                    } else {
                        STATUS = iDent;
                        stopLoop = true;
                    }
                    break;
                }
                case NMChar: {
                    if (isLetter(c) || isDigitalCharacter(c) || c == UNDER_LINE_SIGN || c == IDENT_START_SIGN || (c &0x80)) {
                        STATUS = NMChar;
                    } else if (c == BACK_SPLASH) {
                        STATUS = EscapeStartInNMChar;
                    } else {
                        STATUS = iDent;
                        stopLoop = true;
                    }
                    break;
                }
                case EscapeStartInNMStart: {
                    if (isHexCharacter(c)) {
                        for (int i = 0; i < 5; i++) {
                            c = NextChar(m_buffer);
                            if (c == 0) {break;}
                            if (isHexCharacter(c)) {
                                continue;
                            } else {
                                --m_forwardPos;
                                break;
                            }
                        }
                        STATUS = NMStart;
                    }
                    break;
                }
                case EscapeStartInNMChar: {
                    if (isHexCharacter(c)) {
                        for (int i = 0; i < 5; i++) {
                            c = NextChar(m_buffer);
                            if (c == 0) {break;}
                            if (isHexCharacter(c)) {
                                continue;
                            } else {
                                --m_forwardPos;
                                break;
                            }
                        }
                        STATUS = NMChar;
                    }
                    break;
                }
                default: {
                    STATUS = LexError;
                    break;
                }
            }
            if (stopLoop) {
                --m_forwardPos;
                break;
            }
        }
        token->data = createData(m_firstPos, m_forwardPos);
        if (STATUS == iDent) {
            token->type = IDENT;
        } else if (STATUS == end) {
            token->type = END;
        } else {
            token->type = ERROR;
        }
        m_firstPos = m_forwardPos;
        return token;
    }
    
    Lex::CSSToken* Lex::GetNumberToken()
    {
        enum {_start, _numberStart, _numberFinish, _numberInDecimal, _error};
        std::string s;
        char status = _start;
        char c = NextChar(m_buffer);
        if (c == 0) {return NULL;}
        std::string data;
        while (1) {
            switch (status) {
                case _start: {
                    if (!isDigitalCharacter(c)) {
                        status = _error;
                        break;
                    } else {
                        status = _numberStart;
                        data.append(std::string(1, c));
                    }
                    break;
                }
                case _numberStart: {
                    if (!isDigitalCharacter(c)) {
                        if (c == '.') {
                            status = _numberInDecimal;
                        } else {
                            status = _numberFinish;
                            break;
                        }
                    }
                    data.append(std::string(1, c));
                    break;
                }
                case _numberInDecimal:{
                    if (!isDigitalCharacter(c)) {
                        status = _numberFinish;
                    }
                    data.append(std::string(1, c));
                    break;
                }
                default: {
                    status = _error;
                    break;
                }
            }
            if (status == _numberFinish ||
                status == _error) {
                --m_forwardPos;
                break;
            }
            if (m_bufferSize <= m_forwardPos + 1) {
                status = _numberFinish;
                break;
            }
            c = NextChar(m_buffer);
        }

        if (data.size() == 0) {
            return NULL;
        } else {
            CSSToken* token = new CSSToken;
            m_tokenCache.insert(token);
            token->type = NUMBER;
            token->data = data;
            return token;
        }
    }

    Lex::CSSToken* Lex::GetToken()
    {
        CSSToken* token = new CSSToken;
        m_tokenCache.insert(token);
        token->type = IDENT;
        m_firstPos = m_forwardPos;
        bool stopLoop = false;
        std::string data;
        if (m_firstPos >= m_bufferSize || !m_buffer) {
            token->type = END;
            return token;
        }
        while(1) {
            char c = NextChar(m_buffer);
            switch (m_status) {
                case Start: {
                    switch (c) {
                        case '@': {
                            CSSToken* identToken = GetIdentToken();
                            stopLoop = true;
                            if (identToken->type == IDENT) {
                                m_status = AtKeyWord;
                                data = identToken->data;
                            } else {
                                m_status = LexError;
                            }
                            break;
                        }
                        case '#': {
                            CSSToken* identToken = GetIdentToken();
                            stopLoop = true;
                            if (identToken->type == IDENT) {
                                m_status = Hash;
                                data = identToken->data;
                            } else {
                                m_status = LexError;
                            }
                            break;
                        }
                        case '~': {
                            if (NextChar(m_buffer) == EQUAL_SIGN) {
                                m_status = include;
                            } else {
                                m_status = tidle;
                                --m_forwardPos;
                            }
                            break;
                        }
                        case '|': {
                            if (NextChar(m_buffer) == EQUAL_SIGN) {
                                m_status = dashMatch;
                            } else {
                                m_status = LexError;
                            }
                            break;
                        }
                        case '^': {
                            if (NextChar(m_buffer) == EQUAL_SIGN) {
                                m_status = prefixMatch;
                            } else {
                                m_status = LexError;
                            }
                            break;
                        }
                        case '$': {
                            if (NextChar(m_buffer) == EQUAL_SIGN) {
                                m_status = suffixMatch;
                            } else {
                                m_status = LexError;
                            }
                            break;
                        }
                        case '*': {
                            if (NextChar(m_buffer) == EQUAL_SIGN) {
                                m_status = subStringMatch;
                            } else {
                                --m_forwardPos;
                                m_status = star;
                            }
                            break;
                        }
                        case '"': {
                            m_status = string1Start;
                            break;
                        }
                        case '\'': {
                            m_status = string2Start;
                            break;
                        }
                        case WS_CASE: {
                            m_status = Ws;
                            break;
                        }
                        case '.': {
                            m_status = dot;
                            stopLoop = true;
                            break;
                        }
                        case '{': {
                            m_status = blockStart;
                            stopLoop = true;
                            break;
                        }
                        case '}': {
                            m_status = blockEnd;
                            stopLoop = true;
                            break;
                        }
                        case ',': {
                            m_status = comma;
                            stopLoop = true;
                            break;
                        }
                        case '>': {
                            m_status = greater;
                            stopLoop = true;
                            break;
                        }
                        case '+': {
                            m_status = plus;
                            stopLoop = true;
                            break;
                        }
                        case '[': {
                            m_status = leftSqureBracket;
                            break;
                        }
                        case ']': {
                            m_status = rightSqureBracket;
                            break;
                        }
                        case ':': {
                            m_status = colon;
                            break;
                        }
                        case '=': {
                            m_status = equal;
                            break;
                        }
                        case '/': {
                            char cn = NextChar(m_buffer);
                            if (cn == '*') {
                                m_status = annotationStart;
                            } else {
                                --m_forwardPos;
                                goto identLabel;
                            }
                            break;
                        }
                        case ';' : {
                        	m_status = semicolon;
                        	break;
                        }
                        case NUMBER_CASE: {
                            --m_forwardPos;
                            m_status = numberStart;
                            break;
                        }
                        case ')': {
                            m_status = rightBracket;
                            break;
                        }
                        case '-': {
                            m_status = minus;
                            break;
                        }
                            
                        default: {
                        identLabel:
                            m_forwardPos = m_firstPos;
                            CSSToken* idToken = GetIdentToken();
                            char next = NextChar(m_buffer);
                            if (next == '(') {
                                m_status = function;
                                data = idToken->data;
                            } else {
                                --m_forwardPos;
                                if (idToken->type == IDENT) {
                                    m_status = iDent;
                                    data = idToken->data;
                                } else if (idToken->type == END) {
                                    m_status = end;
                                } else {
                                    m_status = LexError;
                                }
                            }
                            stopLoop = true;
                            break;
                        }
                    }
                    break;
                }
                case Ws: {
                    stopLoop = true;
                    if (isWs(c)) {
                        m_status = Ws;
                        stopLoop = false;
                    } else if (c == PLUS_SIGN) {
                        m_status = plus;
                    } else if (c == COMMA_SIGN) {
                        m_status = comma;
                    } else if (c == GREATER_SIGN) {
                        m_status = greater;
                    } else  if (c == TIDLE_SIGN) {
                        m_status = tidle;
                    } else if ( c == '\0') {
                    	  m_status = end;
                    } else  {
                        --m_forwardPos;
                        m_status = Ws;
                    }
                    break;
                }
                case include: {
                    stopLoop = true;
                }
                case blockStart: {
                    if (c == BLOCK_END_SIGN) {
                        m_status = blockEnd;
                    }
                    while (c != BLOCK_END_SIGN) {
                        if (m_forwardPos >= m_bufferSize) {
                            m_status = LexError;
                            break;
                        } else {
                            c = NextChar(m_buffer);
                            m_status = blockEnd;
                        }
                    }
                    data = createData(m_firstPos, m_forwardPos);
                    stopLoop = true;
                    break;
                }
                case string1Start: {
                    CSSToken* stringToken = GetTextToken(1);
                    if (stringToken->type == STRING) {
                        m_status = string1End;
                        data = stringToken->data;
                    } else {
                        m_status = LexError;
                    }
                    break;
                }
                case string2Start: {
                    CSSToken* stringToken = GetTextToken(2);
                    if (stringToken->type == STRING) {
                        m_status = string2End;
                        data = stringToken->data;
                    } else {
                        m_status = LexError;
                    }
                    break;
                }
                case annotationStart: {
                    char cn = NextChar(m_buffer);
                    if (cn == 0) {break;}
                    if (c == '*' && cn == '/') {
                        m_status = annotationEnd;
                    } else {
                        --m_forwardPos;
                        continue;
                    }
                    break;
                }
                case numberStart: {
                    --m_forwardPos;
                    CSSToken* numberToken = GetNumberToken();
                    if (numberToken) {
                        data = numberToken->data;
                        m_status = num;
                    } else {
                        m_status = LexError;
                    }
                    break;
                }
                    
                default: {
                	stopLoop = true;
                }
                    break;
            }
            bool resetStatus = true;
            switch (m_status) {
                case AtKeyWord: {
                    token->type = ATKEYWORD;
                    token->data = data;
                    break;
                }
                case LexError: {
                    token->type = ERROR;
                    stopLoop = true;
                    break;
                }
                case Hash: {
                    token->type = HASH;
                    token->data = data;
                    break;
                }
                case Ws: {
                    token->type = WS;
                    break;
                }
                case include: {
                    token->type = INCLUDES;
                    stopLoop = true;
                    break;
                }
                case iDent: {
                    token->type = IDENT;
                    token->data = data;
                    break;
                }
                case dot: {
                    token->type = DOT;
                    break;
                }
                case blockStart: {
                    token->type = BLOCKSTART;
                    resetStatus = false;
                    break;
                }
                case blockEnd:{
                    token->type = BLOCKEND;
                    token->data = data;
                    m_status = Start;
                    break;
                }
                case end: {
                    token->type = END;
                    resetStatus = true;
                    break;
                }
                case comma: {
                    token->type = COMMA;
                    break;
                }
                case plus:{
                    token->type = PLUS;
                    break;
                }
                case tidle: {
                    token->type = TIDLE;
                    stopLoop = true;
                    break;
                }
                case greater: {
                    token->type = GREATER;
                    stopLoop = true;
                    break;
                }
                case star:{
                    token->type = STAR;
                    stopLoop = true;
                    break;
                }
                case dashMatch:{
                    token->type = DASHMATCH;
                    stopLoop = true;
                    break;
                }
                case prefixMatch:{
                    token->type = PREFIXMATCH;
                    stopLoop = true;
                    break;
                }
                case suffixMatch:{
                    token->type = SUFFIXMATCH;
                    stopLoop = true;
                    break;
                }
                case includes:{
                    token->type = INCLUDES;
                    stopLoop = true;
                    break;
                }
                case subStringMatch:{
                    token->type = SUBSTRINGMATCH;
                    stopLoop = true;
                    break;
                }
                case leftSqureBracket: {
                    token->type = LEFTSQUREBRACKET;
                    stopLoop = true;
                    break;
                }
                case rightSqureBracket: {
                    token->type = RIGHTSQUREBRACKET;
                    stopLoop = true;
                    break;
                }
                case colon: {
                    token->type = COLON;
                    stopLoop = true;
                    break;
                }
                case equal: {
                    token->type = EQUAL;
                    stopLoop = true;
                    break;
                }
                case semicolon: {
                	token->type =SYNTAXEND;
                	stopLoop = true;
                	break;
                }
                case string1End: case string2End: {
                    token->type = STRING;
                    token->data = data;
                    stopLoop = true;
                    break;
                }
                case annotationEnd: {
                    token->type = ANNOTATION;
                    stopLoop = true;
                    break;
                }
                case function: {
                    token->type = FUNCTION;
                    token->data = data;
                    stopLoop = true;
                    break;
                }
                case numberStart: {
                    stopLoop = false;
                    break;
                }
                case num: {
                    token->data = data;
                    token->type = NUMBER;
                    stopLoop = true;
                    break;
                }
                case rightBracket: {
                    token->type = RIGHTBRACKET;
                    stopLoop = true;
                    break;
                }
                case minus: {
                    token->type = MINUS;
                    stopLoop = true;
                    break;
                }
                default:
                    break;
            }
            if (stopLoop) {
                if (resetStatus) {
                    m_status = Start;
                }
                break;
            }
        }
        return token;
    }
    
    std::string Lex::createData(size_t start, size_t end)
    {
    	if (m_buffer == NULL || start > end) {
    		return NULL;
    	}
    	size_t size = end - start + 1;
    	if (m_bufferSize < end) {
    		return NULL;
    	}
    	char* ptr = new char[size];
    	ptr[size -1] = '\0';
    	memmove(ptr, m_buffer + start, size - 1);
        std::string ret = ptr;
        delete [] ptr;
    	return ret;
    }

    void Lex::CleanResource()
    {
        CleanContainer<CSSToken *>(m_tokenCache);
        delete [] m_buffer;
        m_buffer = 0;
    }
    
    inline bool Lex::isDigitalCharacter(char c) {
        return (c > 0x2F && c < 0x3A);
    }
    
    inline bool Lex::isLetter(char c) {
        return (c > 0x60 && c < 0x7B) || (c > 0x40 && c < 0x5B);
    }
    
    inline bool Lex::isHexCharacter(char c) {
        return isDigitalCharacter(c) || (c > 0x60 && c < 0x67) || (c > 0x40 && c < 0x47);
    }
    
    inline bool Lex::isWs(char c) {
        return (c == ' ' || c == '\r' || c == '\n' || c == '\f' || c == '\t');
    }
}
