# include <stdio.h>
# include <bitset>
# include <iostream> // cin cout
# include <stdlib.h> // atoi
# include <string.h>
# include <vector>
# include <fstream>
#include <iomanip>            // setw
# include <algorithm> // sscanf transform

using namespace std ;

struct Table1 {
	string name = "\0" ;
	int format1 = 0 ;
	int format2 = 0 ;
	string opcode = "\0" ;
};

struct Tables {
	string name = "\0" ;
	int addr = -1 ;
};

struct Token {
	string name ;
	int i = 0 ; // 第幾行指令
	int addr = 0 ; // address
	int tablenum = 0 ;
	int tnum = 0 ;
	bool Isformat4 = false ; // 給SICXE '+' 用
	bool imm = false ; // 給SICXE '#' 用
	bool indirect = false ; // 給SICXE '@' 用
	
	string opcode ;
};

vector <Table1> table1 ;
vector <Tables> table2 ;
vector <string> table3 ;
vector <string> table4 ;

vector <Tables> table5 ; // symbol
vector <Tables> table6 ; // interger
vector <Tables> table7 ; // string
vector <Tables> table9 ; // table8是存instruct set(table1)需要的format和opcode資訊

vector <Token> tokens ;


class GetToken {
	public:
		
		int temploc = -1 ;
		int base0 = -1 ;
		
		bool Is_num ( int &num ) {                                             // command防呆 (排除數字以外的輸入)
   			string numstr ;
   			cin >> numstr ;
			if ( numstr == "0" ) {                                             // 輸入本身為0
				num = 0 ;
	 			return true ;
 			} // if

			num = atoi( numstr.c_str() ) ;
  			if ( num == 0 ) return false ;
  			else return true ;
		} // Is_num()

		void prepare_table ( bool & first_time ) {
			temploc = -1 ;
			base0 = -1 ;
	        fstream file ;
			string filename ;
			Table1 temp ;
			Tables temp1 ;
			string tt ;
			char t[3] ; // 讀 " / " 需要3格

			if ( first_time ) { //--------------------------------------------- 載入固定Table1-4, 初始化其他Table

			    filename = "Table1.table" ;
				file.open( filename, ios::in ) ; // file->table1
				while ( getline( file, temp.name ) ) table1.push_back(temp) ;


				file.close() ;
        		filename = "Table8.table" ;
				file.open( filename, ios::in ) ; // file->table8

        		for ( int i = 0 ; i < table1.size() ; i++ ) {
		    	    file >> table1[i].format1 ;

					if ( file.peek() == ' ' ) {
					    file.read( t, sizeof(t) ) ;
						file >> table1[i].format2 ;
					} // if

					file >> table1[i].opcode ;

				} // for
				file.close() ;

				filename = "Table2.table" ;
				file.open( filename, ios::in ) ;
				while ( getline( file, temp1.name ) ) table2.push_back( temp1 ) ;
				file.close() ;

				filename = "Table3.table" ;
				file.open( filename, ios::in ) ;
				while ( getline( file, tt ) ) {
					if ( tt.compare("A ") == 0 ) tt = "A" ;
					table3.push_back( tt ) ;
				} // while
				file.close() ;

				filename = "Table4.table" ;
				file.open( filename, ios::in ) ;
				while ( getline( file, tt ) ) {
					if ( tt.compare("- ") == 0 ) tt = "-" ;
					table4.push_back( tt ) ;
				} // while
				file.close() ;


				first_time = false ;
			} // if -------------------------------------------
			else { // ------------------------------------------------------ 清除上一筆資料建立的table
			    table5.clear() ;
		 		table6.clear() ;
				table7.clear() ;
				table9.clear() ;
				tokens.clear() ;
			} // else
			
			table5.resize(100) ;
			table6.resize(100) ;
			table7.resize(100) ;
			table9.resize(100) ;

		} // prepare_table()

		void check ( string & buffer, int t ) {
			Token temp ;
			temp.i = t ; //這是第幾行指令的token
			temp.name = buffer ;
			
			if ( buffer[0] == '+' ) { // format4
				temp.Isformat4 = true ;
				buffer = buffer.substr( 1, buffer.size() - 1 ) ;
			} // if
			if ( buffer[0] == '#' ) { // imm
				temp.imm = true ;
				buffer = buffer.substr( 1, buffer.size() - 1 ) ;
			} // if
			if ( buffer[0] == '@' ) { // indirect
				temp.indirect = true ;
				buffer = buffer.substr( 1, buffer.size() - 1 ) ;
			} // if

			string bb = buffer ;
			transform( bb.begin(), bb.end(), bb.begin(), ::tolower ) ; // 這是小寫
			string b = buffer ;
			transform( b.begin(), b.end(), b.begin(), ::toupper ) ; // 這是大寫
			
//若新增table就要多加進來
			for ( int i = 0 ; ( i < table1.size() || i < table2.size() || i < table3.size() || i < table4.size()
				  	  	  	  				  || i < table5.size() || i < table6.size() || i < table7.size() )
				  	  	  	  				  && buffer.compare( "\0" ) != 0 ; i++ ) {
				if ( i < table1.size() && bb.compare( table1[i].name ) == 0  ) {
					temp.tablenum = 1 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // if
				else if ( i < table2.size() && b.compare( table2[i].name ) == 0 ) {
					temp.tablenum = 2 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table3.size() && b.compare( table3[i] ) == 0 ) {
					temp.tablenum = 3 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table4.size() && buffer.compare( table4[i] ) == 0 ) {
					temp.tablenum = 4 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table5.size() && buffer.compare( table5[i].name ) == 0 ) {
					temp.tablenum = 5 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table6.size() && buffer.compare( table6[i].name ) == 0 ) {
					temp.tablenum = 6 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table7.size() && buffer.compare( table7[i].name ) == 0 ) {
					temp.tablenum = 7 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
				else if ( i < table9.size() && buffer.compare( table9[i].name ) == 0 ) {
					temp.tablenum = 9 ;
					temp.tnum = i ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if

			} // for 先檢查"所有"Table有沒有這個token
			
			if ( buffer.compare("") != 0 ) {

			    Tables u ;
				int num = 0 ;
				int hashnum = 0 ; // hash過的num
				for ( int i = 0 ; i < sizeof(buffer) ; i++ ) num = num + buffer[i] ; // hash table 要用
			
				if ( all_of(buffer.begin(), buffer.end(), ::isdigit) ) { // interger
				    u.name = buffer ;
					hashnum = num%100 ;
					if ( table6[hashnum].name.compare( "\0" ) != 0 ) {

				        for ( ; table6[hashnum].name != "\0" ; hashnum++ ) { // hash
						    if ( hashnum == 100 ) hashnum = 0 ;
					    } // for

				    } // if

				 	table6[hashnum] = u ;
					
					temp.tablenum = 6 ;
					temp.tnum = hashnum ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // if
				else if ( buffer[1] == '\'' && buffer[buffer.size()-1] == '\'' ) { // string
					u.name = buffer ;
					hashnum = num%100 ;
					if ( table7[hashnum].name.compare( "\0" ) != 0 ) {

					    for ( ; table7[hashnum].name != "\0" ; hashnum++ ) { // hash
						    if ( hashnum == 100 ) hashnum = 0 ;
					    } // for

					} // if

					table7[hashnum] = u ;

					temp.tablenum = 7 ;
					temp.tnum = hashnum ;
					tokens.push_back(temp) ;
					buffer = "" ;
			    } // else if
			    else if ( buffer[0] == '=' ) { // literal
			    	u.name = buffer ;
					hashnum = num%100 ;
					if ( table9[hashnum].name.compare( "\0" ) != 0 ) {

					    for ( ; table9[hashnum].name != "\0" ; hashnum++ ) { // hash
						    if ( hashnum == 100 ) hashnum = 0 ;
					    } // for

				  	} // if

					table9[hashnum] = u ;

					temp.tablenum = 9 ;
					temp.tnum = hashnum ;
					tokens.push_back(temp) ;
					buffer = "" ;
				} // else if
			    else { // 如果buffer是symbol/label
				    u.name = buffer ;
					hashnum = num%100 ;
					if ( table5[hashnum].name.compare( "\0" ) != 0 ) {
					    for ( ; table5[hashnum].name != "\0" ; hashnum++ ) { // hash
						    if ( hashnum == 100 ) hashnum = 0 ;
				   	    } // for

				    } // if
				
					table5[hashnum] = u ;

					temp.tablenum = 5 ;
					temp.tnum = hashnum ;
					tokens.push_back(temp) ;
					buffer = "" ;
				
			    } // else
			
			} //if
			
		} // check()

		bool syntax( int &location, int &now, int command, bool & IsEQU ) { // 判斷文法+設位址
		
			for ( int i = now ; i < tokens.size() ; i++ ) { // 判斷文法
				if ( tokens[i].tablenum == 1 || tokens[i].tablenum == 2 ) {
				    if ( !(i-now <= 1) ) {
				    	cout << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
					    cout << "Syntax error ! :)" << endl ;
					    return false ;
				    } // if
				        
			    } // if
			    else if( tokens[i].tablenum == 2 ) {
			    	if ( (tokens[i].tnum == 0 || tokens[i].tnum == 2 || tokens[i].tnum == 3 || tokens[i].tnum == 4
						   || tokens[i].tnum == 5 || tokens[i].tnum == 6 || tokens[i].tnum == 7 ) && i != tokens.size() - 2 ) {
						   	cout << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
							cout << "syntax error ! :)" << endl ;
							return false ;
					} // if
					else if ( tokens[i+1].tnum == 8 && i != tokens.size()-1 ) {
						cout << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
						cout << "syntax error ! :)" << endl ;
						return false ;
					} // else if
					else if ( i == tokens.size()-2 && tokens[i+1].tablenum != 5
                            && tokens[i+1].tablenum != 6 && tokens[i].tnum != 6 ) {
                        cout << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
						cout << "syntax error ! :)" << endl ;
						return false ;
					} // else if
					else if ( tokens[i].tnum == 4 || tokens[i].tnum == 5 ) {
						if ( tokens[i+1].tablenum == 5 ) {
							cout << "syntax error ! :)" << endl ;
							return false ;
						} // if
					} // else if
				} // else if
				if (command == 1 ) { // SIC
				    if ( tokens[i].tablenum == 1 ) { // 如果tokens是指令
				    	if ( tokens[i].tnum == 0 || tokens[i].tnum == 3 || tokens[i].tnum == 5 || tokens[i].tnum == 8
						   || tokens[i].tnum == 14 || tokens[i].tnum == 15 || tokens[i].tnum == 16 || tokens[i].tnum == 17
						   || tokens[i].tnum == 18 || tokens[i].tnum == 19 || tokens[i].tnum == 21 || tokens[i].tnum == 23
						   || tokens[i].tnum == 26 || tokens[i].tnum == 28 || tokens[i].tnum == 32 || tokens[i].tnum == 33
						   || tokens[i].tnum == 35 || tokens[i].tnum == 40 || tokens[i].tnum == 42 || tokens[i].tnum == 45
						   || tokens[i].tnum == 47 || tokens[i].tnum == 49 || tokens[i].tnum == 50 || tokens[i].tnum == 54
						   || tokens[i].tnum == 56 || tokens[i].tnum == 58 ) {
						   	if ( tokens[i].tnum == 35 ) { // RSUB
								if ( i != tokens.size()-1 ) {
									cout << "1 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
						   			cout << "Syntax error ! :)" << endl ;
					    			return false ;
								} // if
							} // if
							else {
								if ( i == tokens.size()-1 ) {
									cout << "2 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
									cout << "Syntax error ! :)" << endl ;
					    			return false ;
								} // if
								else if ( i == tokens.size()-2 ) {
									if ( tokens[i+1].tablenum != 5 ) {
									    cout << "3 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
										cout << "Syntax error ! :)" << endl ;
					    				return false ;
					    			} // if
								} // else if
								else if ( i == tokens.size()-3 ) {
									if ( !(tokens[i+1].tablenum == 5 && tokens[i+2].tablenum == 3 && tokens[i+2].tnum == 1) ) {
									    cout << "4 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
										cout << "Syntax error ! :)" << endl ;
					    				return false ;
					    			} // if
								} // else if
							} // else
						} // if
						else {
							cout << "* " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
							cout << "Not SIC program ! :)" << endl ;
							return false ;
						} // else
						 
					} // if
					else if ( tokens[i].tablenum == 3 ) { // 如果tokens是X
						 if ( tokens[i].tnum != 1 || i != tokens.size() -1 ) {
						 	cout << "5 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
						 	cout << "syntax error ! :)" << endl ;
							return false ;
						 } // if
				    } // else if
				} // if
				else if ( command == 2 ) { // SICXE
					if ( tokens[i].tablenum == 1 ) { // 如果tokens是指令
						if ( table1[tokens[i].tnum].format1 == 1 ) { // format1
						    if ( i != tokens.size()-1 ) {
						        cout << "6 " << tokens[i].i <<  tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
						    	cout << "syntax error ! :)" << endl ;
						    	return false ;
						    } // if
					    } // if
					    else if ( table1[tokens[i].tnum].format1 == 2 ) { // format2
					    	if ( tokens[i].tnum == 4 || tokens[i].tnum == 57 ) { // r1
					    		if ( !( i == tokens.size() - 2 && tokens[i+1].tablenum == 3 ) ) {
					    			cout << "7 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
					    			cout << "syntax error ! :)" << endl ;
						    		return false ;
								} // if
							} // if
							else if ( tokens[i].tnum == 36 || tokens[i].tnum == 37 ) { // ri, n
								if ( i == tokens.size() - 2 ) {
									if ( tokens[i+1].tablenum != 3 ) {
										cout << "8 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
										cout << "syntax error ! :)" << endl ;
						    			return false ;
									} // if
								} // if
								else if( i == tokens.size() - 3 ) {
									if ( !(tokens[i+1].tablenum == 3 && (tokens[i+2].tablenum == 5||tokens[i+2].tablenum == 6))){
										cout << "9 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
										cout << "syntax error ! :)" << endl ;
						    			return false ;
									} //if
								} // else if
							} // else if
							else if ( tokens[i].tnum == 53 ) { // n
								if ( !(i == tokens.size()-2 && (tokens[i+1].tablenum == 5 || tokens[i+1].tablenum == 6 )) ){
		 						    cout << "10 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
									cout << "syntax error ! :)" << endl ;
						    		return false ;
								} // if
							} // else if
							else {
								if ( !(i == tokens.size()-3 && tokens[i+1].tablenum == 3 && tokens[i+2].tablenum == 3 )) {
									cout << "11 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
									cout << "syntax error ! :)" << endl ;
						    		return false ;
								} // if
							} // else
						} // else if
						else if ( table1[tokens[i].tnum].format1 == 3 ) { // format3 / 4
							if ( i == tokens.size()-3 ) {
								if ( !( tokens[i+1].tablenum == 5 && tokens[i+2].tablenum == 3 && tokens[i+2].tnum == 1 ) ) {
									cout << "12 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
									cout << "syntax error ! :)" << endl ;
						    		return false ;
								} // if
							} // if
							else if ( i == tokens.size()-2 ) {
								if ( !( tokens[i+1].tablenum == 5 || tokens[i+1].tablenum == 6 ) ) {
									cout << "13 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
									cout << "syntax error ! :)" << endl ;
						    		return false ;
								} // if
							} // else if
						} // else if
					    
				    } // if
				    else if ( tokens[i].tablenum == 2 ) { // 如果是Pseudo
				    	if ( tokens[i].tnum == 2 || tokens[i].tnum == 3 ) {
				    		if ( tokens[i+1].tablenum == 5 && tokens[i+1].name[0] != '=' ) {
				    			cout << "14 " << tokens[i].i << tokens[i].name << setw(6) << tokens[i].tablenum << endl ;
				    			cout << "syntax error ! :)" << endl ;
						    	return false ;
							} // if
						} // if
					} // else if
				} // else if
			} // for
		
		
			for (  int i = now ; tokens[i].i == tokens[now].i ; i++ ) { // 設位址
				if ( tokens[i].tablenum == 1 || ( tokens[i].tablenum == 2 && tokens[i].tnum == 3 ) ) {
					if ( command == 2 && tokens[i].tablenum == 1 && tokens[i].name[0] == '+' ) location = location+4 ;
					else if ( command == 2 && tokens[i].tablenum == 1 && table1[tokens[i].tnum].format1 == 1 ) location++ ;
					else if ( command == 2 && tokens[i].tablenum == 1 && table1[tokens[i].tnum].format1 == 2) location=location+2;
					else location = location + 3 ;
					return true ;
				} // if
				else if ( tokens[i].tablenum == 2 ) {
					if ( i == now && tokens[i].tnum == 0 ) { // START
						return true;
					} // if
					else if ( tokens[i].tnum == 6 ) { // EQU
						IsEQU = true ;
						if ( tokens[i+1].tablenum == 6 ) {
							temploc = stoi(tokens[i+1].name) ;
							return true ;
						} // if
						else if ( tokens[i+1].tablenum == 5 ) {
							temploc = -2 ;
							return true ;
						} // else if
						
						//+-
						
					} // else if
					else if ( tokens[i].tnum == 2 ) {
						if ( tokens[i+1].name[0] == 'C' ) location = location + tokens[i+1].name.size() - 3 ;
						else if ( tokens[i+1].name[0] == 'X' ) location ++ ;
						else location++ ;
						return true;
					} // else if
					else if ( tokens[i].tnum == 4 || tokens[i].tnum == 5 ) {
						if (tokens[i].tnum == 4 && tokens[i+1].tablenum == 6)
						   location = location + stoi(tokens[i+1].name);
						if (tokens[i].tnum == 5 && tokens[i+1].tablenum == 6)
						   location = location + 3*stoi(tokens[i+1].name);
						return true;
					} // else if
					else if ( tokens[i].tnum == 7 || tokens[i].tnum == 8 || tokens[i].tnum == 1 ) { //BASE LTORG END
						if ( tokens[i].tnum == 7 ) base0 = i ;
						if ( tokens[i].tnum == 8 || tokens[i].tnum == 1 ) { // 設literal addr
							for ( int i = 0 ; i < table9.size() ; i++ ) {
								if ( table9[i].name.compare("\0") != 0 && table9[i].addr != -1 ) {
									if ( all_of(table9[i].name.begin()+1, table9[i].name.end(), ::isdigit) ) { // =int
										table9[i].addr = location ;
										location = location+3 ;
									} // if
									else { // =C / =X
										table9[i].addr = location ;
										location = location+3 ;
									} // else if
								} // if
							} // for
						} // if
					
						IsEQU = true ;
						temploc = -1 ;
						return true;
					} // else if
					else {
						cout << "syntax error! :)" << endl ;
						return false ;
					} // else
						
				} // else if
				else if ( tokens[i].tablenum == 5 ) {
					if ( i == now  ) { // 設 Pseudo addr
					    if ( table5[tokens[i].tnum].addr == -1 ) table5[tokens[i].tnum].addr = location ;
					    else {
					    	cout << "Duplicate definition error ! :)" << endl ;
					    	return false ;
						} // else
					} // if
						
				} // else if
					
			} // for

			
		} // syntax

		void gettoken( int command ) {
			bool IsEQU = false ;
			Token temp ;
			int location ;
			int now = 0 ;
			string str ;
			string buffer ;
			string filename ;
			cout << endl << "Input the file name: [0]Quit " ;
			cin >> filename ;
			if ( filename.compare("0") == 0 ) return ;
			
			fstream file ;
			file.open( filename.c_str(), ios::in ) ;
			
			if ( !file ) {
			  	cout << endl << filename << " does not exist!!!" << endl ;
			  	gettoken( command ) ;
	        } // if
			else {
				file >> str ;
				if ( str.compare( "COPY" ) == 0 ) file >> str ; // "COPY START  1000"
				if ( str.compare( "START" ) == 0 ) file >> str ;
				if ( sscanf( str.c_str(), "%X", &location ) ) {
				     // 將16進制字串轉換成10進制整數
					if ( file.peek() == '.' ) getline( file, str, '\n' ) ; // 如果後面有註解，全部讀掉
				} // if
				else {
					cout << endl << "Lexical error !" << endl ;
					file.close() ;
					return ;
				} // else
				
// 以下開始 切token + 判斷token + 處理命令位址(location)
				getline( file, str, '\n' ) ;
				for ( int t = 1 ; getline( file, str, '\n' ) ; t++ ) { // 一次讀一行
					buffer = "" ;
					now = tokens.size() ;
			    	for ( int i = 0 ; str[i] != '\0' ; i++ ) { // 遍歷
				        if ( str[i] == '\t' || str[i] == sizeof(str) || str[i] == ',' || str[i] == ' ' ) {
				        	if ( buffer.compare("") != 0 ) {
				        		check( buffer, t ) ;
				        		if ( tokens[tokens.size()-1].name.compare("END") == 0 ) break ;
							} // if
				        	buffer = "" ;
						} // if
						else if ( str[i] == '.' ) {
							buffer = "" ;
							cout << endl ;
							break ;
						} // else if
						else buffer = buffer + str[i] ;
						
			    	} // for
			    	
			    	
			    	if ( buffer.compare("") != 0 ) {
					    check( buffer, t ) ;

					    for ( int i = tokens.size()-1 ; tokens[i].i == tokens[tokens.size()-1].i ; i-- ) { // 設addr
							tokens[i].addr = location ;
						} // for
						
						if( !syntax( location, now, command, IsEQU ) ) {
							cout << "syntax error! :)" << endl ;
							break ;
						} // if
						
						if (IsEQU) {
							
							
							
						    for ( int i = tokens.size()-1 ; tokens[i].i == tokens[tokens.size()-1].i ; i-- ) // 為了EQU
							    tokens[i].addr = temploc ;
							if ( tokens[tokens.size()-3].i == tokens[tokens.size()-1].i
							   	 						   &&  tokens[tokens.size()-3].tablenum == 5 ) //如果EQU前面有個Pseudo:)
								table5[tokens[tokens.size()-3].tnum].addr = temploc ;
						} // if
						IsEQU = false ;
					} // if
			    	cout << tokens[tokens.size()-1].name << " " << tokens[tokens.size()-1].addr << endl ;
		    	} // for

		    	for ( int i = 0 ; i < table5.size() ; i++ ) {
		    		
				    if ( table5[i].name.compare("\0") != 0 && table5[i].addr == -1 ) {
				    	cout << table5[i].name << " " << hex << table5[i].addr << endl ;
				    	cout << "Undefine symbol error ! :)" << endl ;
				    	return ;
					} // if
		    	
		    	} // for
		    	cout << endl ;

		    	for ( int i = 0 ; i < tokens.size() ; i++ ) {
		    		if ( i != 0 && tokens[i].i != tokens[i-1].i )
					cout << setw(5) << hex << tokens[i].addr << setw(8) <<tokens[i].name << endl ;
				} // for
				
				
		    	
			} // else
			

	    } // gettoken()
	
}; // GetToken()

class SIC : public GetToken {
	public :
		void assembler () {
			string t6 ;
			for ( int i = 0 ; i < table6.size() ; i++ ) {
				t6 = "" ;
				if ( table6[i].name.compare( "" ) != 0 ) {
					for ( int p = 0 ; p < table6[i].name.size() ; p++ ) {
						if ( table6[i].name[p]-'0' <= 9 && table6[i].name[p] >= 0  )t6 = t6 + table6[i].name[p] ;
					} // for
					table6[i].addr = stoi(t6) ;
				} // if
			} // for

			if ( base0 != -1 ) {
				if ( tokens[base0].tablenum == 5 ) base0 = table5[tokens[base0].tnum].addr ;
				if ( tokens[base0].tablenum == 6 ) base0 = table6[tokens[base0].tnum].addr ;

			} // if
			
			int itemp ;
			string s ;
			string str ;
			char pp[8] ;
			for ( int i = 0 ; i < tokens.size() ; i++ ) {
				if ( tokens[i].tablenum == 1 ) {
					if ( tokens[i].tnum != 35 ) {
					    itemp = -1 ;
						for ( int j = 0 ; j < table5.size() ; j++ ) {
						    if ( table5[j].name.compare( tokens[i+1].name ) == 0 ) itemp = table5[j].addr ;
						} // for
						
						if ( itemp == -1 ) {
						    cout << "Undefine symbol error ! :)" << endl ;
							return ;
						} // if
						sprintf( pp, "%04X", itemp ) ;
						str = pp ;
						str = table1[tokens[i].tnum].opcode + str ;
						sscanf( str.c_str(), "%06X", &itemp ) ;
						if ( tokens[i+2].addr == tokens[i].addr ) itemp = itemp + 32768 ; // x = 1 -> dec + 32768(2^15)
						sprintf( pp, "%06X", itemp ) ;
						str = pp ;
						tokens[i].opcode = str ;
						str = "\0" ;
					    cout << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // if
					else {
						tokens[i].opcode = "4C0000" ;
						cout << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // else
				} // if
				else if ( tokens[i].tablenum == 2 &&
					      ( tokens[i].tnum == 2 || tokens[i].tnum == 3 || tokens[i].tnum == 4 || tokens[i].tnum == 5 ) ) {
					if ( tokens[i+1].tablenum == 6 ) { // int
						itemp = stoi( tokens[i+1].name ) ;
						sprintf( pp, "%06X", itemp ) ;
						str = pp ;
						tokens[i].opcode = str ;
						str = "\0" ;
						cout << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // if
					else {
						if ( tokens[i+1].name[0] == 'X' || tokens[i+1].name[0] == 'x' ) {
							str = tokens[i+1].name ;
							str.erase( 0, 2 ) ;
							str.erase( str.size()-1, 1 ) ;
						} // if
						else if ( tokens[i+1].name[0] == 'C' || tokens[i+1].name[0] == 'c' ) {
							str = "\0" ;
							for ( int j = 2 ; j < tokens[i+1].name.size()-1 ; j++ ) {
								itemp = tokens[i+1].name[j] ;
								sprintf( pp, "%02X", itemp ) ;
								s = pp ;
								str = str + s ;
							} // for
							s = "\0" ;
						} // else if
						tokens[i].opcode = str ;
						str = "\0" ;
						cout << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // else
				} // else if
			} // for
			
			//make file----------------------
			string filename ;
			int addrre ;
			string set ;
			string op ;
			int temp ;
			int k = 1 ;
			cout << endl << endl << "Make file name: " ;
			cin >> filename ;
			filename = "Output" + filename + ".txt" ;
			
			FILE * outfile ;
			outfile = fopen( filename.c_str(), "w" ) ;
			fprintf(outfile, "    Line      Loc            Source statement           Object code  \n" );
			fprintf(outfile, "  ------      ---              ----------------               -----------  \n" );
			fprintf(outfile, "0       0000    START    %04X\n", tokens[0].addr ) ;
			temp = tokens[0].i ;
			addrre = tokens[0].addr;
			for ( int i = 0 ; i < tokens.size() ; i++ ) {

				if ( tokens[i].i != temp ) {
					fprintf( outfile, "%-15d", temp ) ;
					fprintf( outfile, "%04X", addrre ) ;
					fprintf( outfile, "%40s", set.c_str() ) ;
					fprintf( outfile, "%20s\n", op.c_str()) ;
					set = "" ;
					addrre = tokens[i].addr ;
					temp = tokens[i].i ;
				} // if
					if ( tokens[i].tablenum == 2 || tokens[i].tablenum == 1 ) {
						op = tokens[i].opcode ;
					} // if



				set = set + tokens[i].name + "    " ;
			} // for
			fprintf( outfile, "      END" ) ;
			fclose(outfile) ;
			
		} // assembler()
}; // SIC

class SIC_XE : public GetToken {
	public :
		void assembler () {
			int bt = 0 ;
			string t6 ;
			for ( int i = 0 ; i < table6.size() ; i++ ) {
				t6 = "" ;
				if ( table6[i].name.compare( "" ) != 0 ) {
					for ( int p = 0 ; p < table6[i].name.size() ; p++ ) {
						if ( table6[i].name[p]-'0' <= 9 && table6[i].name[p] >= 0  )t6 = t6 + table6[i].name[p] ;
					} // for
					table6[i].addr = stoi(t6) ;
				} // if
			} // for
			
			if ( base0 != -1 ) {
				if ( tokens[base0].tablenum == 5 ) base0 = table5[tokens[base0].tnum].addr ;
				if ( tokens[base0].tablenum == 6 ) base0 = table6[tokens[base0].tnum].addr ;

			} // if
			
			int r1 = -1 ;
			int r2 = -1 ;
			int btt ;
			
			unsigned int itemp ;
			string s ;
			string str ;
			char pp[8] ;
			for ( int i = 0 ; i < tokens.size() ; i++ ) {
				if ( tokens[i].tablenum == 5 && table5[tokens[i].tnum].addr == -1 ) {
					cout << "Undefined error ! :)" << endl ;
				} // if
				
				if ( tokens[i].tablenum == 1 ) {
					if ( tokens[i].tnum == 35 ) { 
					    tokens[i].opcode = "4E0000" ; //RSUB
						cout << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // if
					else if ( table1[tokens[i].tnum].format1 == 1 ) {
						tokens[i].opcode = table1[tokens[i].tnum].opcode ;
						cout << setw(8) << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // else if
					else if ( table1[tokens[i].tnum].format1 == 2 ) {
						str = table1[tokens[i].tnum].opcode ;
						if ( i < tokens.size()-2 && tokens[i].addr == tokens[i+2].addr ) { // 2個operand
							r1 = 0 ;
							r2 = 0 ;
							if ( tokens[i+1].tablenum == 3 ) r1 = tokens[i+1].tnum ;
							if ( tokens[i+2].tablenum == 3 ) r2 = tokens[i+2].tnum ;
	   						pp[0] = r1 +'0' ;
							pp[1] = r2 +'0' ;
							str = str + pp[0] + pp[1];
							tokens[i].opcode = str ;
							str = "" ;
							cout << setw(8) << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
						} // if
						else if ( i < tokens.size()-1 && tokens[i].addr == tokens[i+1].addr ) { // 1個operand
							r1 = 0 ;
							r2 = 0 ;
							if ( tokens[i+1].tablenum == 3 ) r1 = tokens[i+1].tnum ;
	   						pp[0] = r1 +'0' ;
							str = str + pp[0] + "0" ;
							tokens[i].opcode = str ;
							str = "" ;
							cout << setw(8) << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
						} // else if
						else { // 0個operand
							str = str + "00" ;
							tokens[i].opcode = str ;
							str = "" ;
							cout << "." << setw(8) << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
						} // else if
						
					} // else if
					else if ( table1[tokens[i].tnum].format1 == 3 ) {
						s = table1[tokens[i].tnum].opcode ;
						sscanf( s.c_str(), "%X" , &itemp ) ;
						itemp = itemp+3 ;
						bitset<8> b (itemp) ;
						str = b.to_string() ;// 2進制的op ni
						if ( i < tokens.size()-1 && tokens[i+1].tablenum == 3 && tokens[i+1].tnum == 1 ) str = str + "1" ; // x
						else str = str + "0" ;
						
						if (tokens[i].Isformat4) {
							str = str + "001" ; // bpe
							if ( tokens[i+1].tablenum == 5 ) r1 = table5[tokens[i+1].tnum].addr ;
							if ( tokens[i+1].name[0] == '#' ) {
								str[6] = '0' ;
								if ( tokens[i+1].tablenum == 6 ) r1 = table6[tokens[i+1].tnum].addr ;
							} // if
							if ( tokens[i+1].name[0] == '=' ) {
								if ( tokens[i+1].tablenum == 9 ) r1 = table9[tokens[i+1].tnum].addr ;
							} // if
							if ( tokens[i+1].name[0] == '@' ) {
								str[7] = '0' ;
							} // if
							if ( tokens[i+1].name[0] != '#' ) r1 = r1 + tokens[0].addr ;
							bt = 1 ;
							itemp = 0 ;
							for ( int m = str.size()-1 ; m >= 0 ; m-- ) {
								if (str[m] == '1') itemp = itemp + bt ;
								bt = bt*2 ;
							} // for
							sprintf( pp, "%03x", itemp ) ;
							str = pp ;
							sprintf( pp, "%05x", r1 ) ;
							str = str + pp ;
							tokens[i].opcode = str ;
							str = "" ;
							cout << setw(8) << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
						} // if
						else {
							if( tokens[i+1].name[0] == '#' ) {
								if ( tokens[i+1].tablenum == 6 ) r1 = table6[tokens[i+1].tnum].addr ;
								if ( tokens[i+1].tablenum == 5 ) r1 = table5[tokens[i+1].tnum].addr ;
								r2 = r1 ;
								str = str + "000" ;//bpe
								str[6] = '0' ;//ni
							} // else if
							else if( tokens[i+1].name[0] == '=' ) {
								if ( tokens[i+1].tablenum == 9 ) r1 = table9[tokens[i+1].tnum].addr ;
								str = str + "010" ;//bpe
							} // else if
							else if ( tokens[i+1].name[0] == '@' ) {
								if ( tokens[i+1].tablenum == 5 ) r1 = table5[tokens[i+1].tnum].addr ;
								str = str + "010" ;//bpe
								str[7] = '0' ;//ni
							} // else if
							else if ( tokens[i+1].tablenum == 6 ) {
								r2 = table6[tokens[i+1].tnum].addr ;
								str = str + "000" ;//bpe
							} // else if
							else {
								if ( tokens[i+1].tablenum == 6 ) r1 = table6[tokens[i+1].tnum].addr ;
								if ( tokens[i+1].tablenum == 5 ) r1 = table5[tokens[i+1].tnum].addr ;
							    str = str + "010" ; // bpe 若超範圍再變10
							} // else
							
							if ( str[10] == '1' ) {
								for ( bt = i ; tokens[bt].addr == tokens[i].addr ; bt++ ) ;
								btt = bt ;
								r2 = r1 - tokens[bt].addr ;
								if ( r2 < -2048 || r2 > 2047 ) {
								    str[9] = '1' ;
									str[10] = '0' ;
								} // if
								
							} // if

	   						bt = 1 ;
							itemp = 0 ;
							for ( int m = str.size()-1 ; m >= 0 ; m-- ) {
								if (str[m] == '1') itemp = itemp + bt ;
								bt = bt*2 ;
							} // for
							sprintf( pp, "%03x", itemp ) ;
							str = pp ;
							
							if ( r2 >= 0 ) {
							    sprintf( pp, "%03x", r2 ) ;
							} // if
							else {
								if ( r2 < -2048 || r2 > 2047 ) {
									if ( base0 != -1 ) r2 = base0 ;
							    } // if
							    else r2 = tokens[btt].addr ;
							    
							    bitset<12> b1 (r1) ;
							    bitset<12> b2 (r2) ;
							    b2 = b2.flip() ;
							    
							    bt = 1 ;
								itemp = 0 ;
								s = b2.to_string() ;
								for ( int m = s.size()-1 ; m >= 0 ; m-- ) {
								    if ( s[m] == '1') itemp = itemp + bt ;
									bt = bt*2 ;
							    } // for
							    itemp++ ;
							    r2 = r1 + itemp ;
							    sprintf( pp, "%03x", r2 ) ;
							} // else
							str = str + pp ;
							tokens[i].opcode = str ;
							str = "" ;
							cout << setw(8) << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
						} // else

					} // else if
					
				} // if
				else if ( tokens[i].tablenum == 2 &&
					      ( tokens[i].tnum == 2 || tokens[i].tnum == 3 || tokens[i].tnum == 4 || tokens[i].tnum == 5 ) ) {
					if ( tokens[i+1].tablenum == 6 ) { // int
						itemp = stoi( tokens[i+1].name ) ;
						sprintf( pp, "%06x", itemp ) ;
						str = pp ;
						tokens[i].opcode = str ;
						str = "\0" ;
						cout << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // if
					else {
						if ( tokens[i+1].name[0] == 'X' || tokens[i+1].name[0] == 'x' ) {
							str = tokens[i+1].name ;
							str.erase( 0, 2 ) ;
							str.erase( str.size()-1, 1 ) ;
						} // if
						else if ( tokens[i+1].name[0] == 'C' || tokens[i+1].name[0] == 'c' ) {
							str = "\0" ;
							for ( int j = 2 ; j < tokens[i+1].name.size()-1 ; j++ ) {
								itemp = tokens[i+1].name[j] ;
								sprintf( pp, "%02x", itemp ) ;
								s = pp ;
								str = str + s ;
							} // for
							s = "\0" ;
						} // else if
						tokens[i].opcode = str ;
						str = "\0" ;
						cout << tokens[i].opcode << setw(8) << tokens[i].name << endl ;
					} // else
				} // else if
			
			} // for
			
			//make file----------------------------------------------------------------------------------------------------
			
			
			string filename ;
			int addrre ;
			string set ;
			string op ;
			int temp ;
			int k = 1 ;
			cout << endl << endl << "Make file name: " ;
			cin >> filename ;
			filename = "Output" + filename + ".txt" ;
			
			FILE * outfile ;
			outfile = fopen( filename.c_str(), "w" ) ;
			fprintf(outfile, "    Line      Loc            Source statement           Object code  \n" );
			fprintf(outfile, "  ------      ---              ----------------               -----------  \n" );
			fprintf(outfile, "0       0000    START    %04X\n", tokens[0].addr ) ;
			temp = tokens[0].i ;
			addrre = tokens[0].addr;
			for ( int i = 0 ; i < tokens.size() ; i++ ) {
				
				if ( tokens[i].i != temp ) {
					fprintf( outfile, "%-15d", temp ) ;
					fprintf( outfile, "%04X", addrre ) ;
					fprintf( outfile, "%40s", set.c_str() ) ;
					fprintf( outfile, "%20s\n", op.c_str()) ;
					set = "" ;
					addrre = tokens[i].addr ;
					temp = tokens[i].i ;
				} // if
					if ( tokens[i].tablenum == 2 || tokens[i].tablenum == 1 ) {
						op = tokens[i].opcode ;
					} // if

				
				
				set = set + tokens[i].name + "    " ;
			} // for
			fprintf( outfile, "      END" ) ;
			fclose(outfile) ;

			
			
			
			
			
			
			
			
		} // assembler()
}; // SIC_XE


int main() {
	GetToken p ;
	SIC sic ;
	SIC_XE sicxe ;
	int command ;
	ifstream file ;
	string filename ;
	bool first_time = true ; // 第一次組譯

	while( true ) {
		cout << "***********************************************" << endl ;
		cout << " System Program Exercise                      *" << endl ;
		cout << " Welcome my Assembler  :)                     *" << endl ;
		cout << " 1.    SIC assembler                          *" << endl ;
		cout << " 2. SIC/XE assembler                          *" << endl ;
		cout << "***********************************************" << endl << endl ;
		cout << "Input a choice (0, 1, 2) : " ;

 		p.prepare_table( first_time ) ;

  	  	if ( !p.Is_num( command ) ) cout << "Command does not exist!" ;
  	 	else if ( command > 2 || command < 0 ) cout << "Command does not exist!" ;
 		else if ( command == 0 ) break ;
  	  	
  	  	else {
  	  		p.gettoken(command) ;
        } // else

		if ( command == 1 ) sic.assembler() ;
		else if ( command == 2 ) sicxe.assembler() ;
		
		cout << endl << endl ;
	} // while
} // main()
