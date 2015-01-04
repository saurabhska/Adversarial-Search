#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<queue>
#include<stack>
#include <sstream>
#include <limits.h>
using namespace std;

int inEvaluationMatrix[8][8]={
	{99,-8,8,6,6,8,-8,99},
	{-8,-24,-4,-3,-3,-4,-24,-8},
	{8,-4,7,4,4,7,-4,8},
	{6,-3,4,0,0,4,-3,6},
	{6,-3,4,0,0,4,-3,6},
	{8,-4,7,4,4,7,-4,8},
	{-8,-24,-4,-3,-3,-4,-24,-8},
	{99,-8,8,6,6,8,-8,99}
};

class piece
{
public:
	int type;
	int row;
	int col;
	int evaluation; //evaluated value by minimax
	int weight;//inEvaluationMatrix values
	int srcRow;
	int srcCol;
	piece()
	{
		type=-1;
		row=-1;
		col=-1;
		evaluation=-1;
		srcRow=-1;
	    srcCol=-1;
	}
};

struct sortMoves {
    bool operator()(piece const & p1, piece const & p2) {
        // return "true" if "p1" is ordered before "p2", for example:
			if (p1.row < p2.row)
				return false;
			else if (p1.row > p2.row)
				return true;
			else if (p1.row == p2.row)
			{
				if (p1.col <= p2.col)
					return false;
				else 
					return true;
			}
		}
};

struct sortBestMoves {
    bool operator()(piece const & p1, piece const & p2) {
        // return "true" if "p1" is ordered before "p2", for example:
		if (p1.evaluation > p2.evaluation)
			return false;
		else if (p1.evaluation < p2.evaluation)
			return true;
		else if (p1.evaluation == p2.evaluation)
		{
			if (p1.row < p2.row)
				return false;
			else if (p1.row > p2.row)
				return true;
			else if (p1.row == p2.row)
			{
				if (p1.col <= p2.col)
					return false;
				else 
					return true;
			}
		}
	}
};


class ReadInput
{
public:
	int inAlgoType;             //type of algo to exec
	string t_inAgtPlyrSymbol;     // char representation of agent/max player   
	int inAgtPlyrSymbol;        // int representation of agent/max player 
	int inOtherPlyrSymbol;        //Other player symbol
	int inCutOffDepth;         //cutoff depth
	int inCurrentState[8][8];  //Current state matrix

	ReadInput()
	{
		inAlgoType=-1;
		t_inAgtPlyrSymbol="";
		inAgtPlyrSymbol=-1;
		inOtherPlyrSymbol=-1;
		inCutOffDepth=-1;
	}

	//Get number equivalent of input char
	int getNumberStr(string input)
	{
		if (input=="O")
			return 0;
		else if (input=="X")
			return 1;
		else 
			return -1;
	}

	int getNumber(char input)
	{
		if (input=='O')
			return 0;
		else if (input=='X')
			return 1;
		else 
			return -1;
	}

	char getSymbol(int input)
	{
		if (input==0)
			return 'O';
		else if (input==1)
			return 'X';
		else 
			return '*';
	}

	void initialize()
	{
		ifstream infile;
		string line;
		int i,j;
		infile.open("input.txt");

		infile >> line;
		inAlgoType = atoi(line.c_str());  //type of algo

		infile >> line;
		t_inAgtPlyrSymbol=line;  
		inAgtPlyrSymbol=getNumberStr(t_inAgtPlyrSymbol); //plyr symbol
		if (inAgtPlyrSymbol==0)
			inOtherPlyrSymbol=1;
		else if (inAgtPlyrSymbol==1)
			inOtherPlyrSymbol=0;

		infile >> line;
		inCutOffDepth = atoi(line.c_str());   //depth

		/*inCurrentState = new int*[8];
		for(i = 0; i < 8; i++)
			inCurrentState[i] = new int[8];*/

		for (i = 0; i < 8; i++)  //current state
		{
			infile >> line;
			for (j = 0; j < 8; j++)
			{
				inCurrentState[i][j]=getNumber(line.at(j));
						
			}
			
		}
		
	}

	void indisplay()
	{
		int i,j;
		ofstream myfile("output.txt");
		//cout << "Algorithm type: "<<inAlgoType<<"\n";
		//cout << "Player Symbol: "<<t_inAgtPlyrSymbol<<"\n";
		//cout << "Equivalent Int Player Symbol: "<<inAgtPlyrSymbol<<"\n";
		//cout << "CutOffDepth: "<<inCutOffDepth<<"\n";

	/*	for (i = 0; i < 8; i++)  //current state
		{
			for (j = 0; j < 8; j++)
			{
				cout<<inCurrentState[i][j]<<"  ";
						
			}
			cout<<"\n";
			
		} */
		
		for (i = 0; i < 8; i++)  //current state
		{
			for (j = 0; j < 8; j++)
			{
				myfile<<getSymbol(inCurrentState[i][j]);
						
			}
			myfile<<"\n";
			
		}
		myfile.close();
	}



};

class GamePlay
{
public:
	ReadInput input; //stores the scanned input 
	vector<piece> inTypePiecesOnBoard; //stores a type of pieces on board computed via findPieces()
	vector<piece> legalMovesOnBoard; //stores legal moves on board via computeLegalMoves()
	
	priority_queue<piece, vector<piece>, sortBestMoves> BestMovequeue;
	
	string NumtoString(int num)
	{
		//string t=num+"";
		ostringstream ss;
		ss<<num;
		if (num==INT_MAX)
			return "Infinity";
		else if (num==INT_MIN)
			return "-Infinity";
		else
			return ss.str();
	}

	void findPieces(int pieceType) //find all pieces of a type on board
	{
	
		piece tempPiece;
		for (int i=0;i<8;i++)
		{
			for (int j=0;j<8;j++)
			{
				if (input.inCurrentState[i][j]==pieceType)
				{
					tempPiece.row=i;
					tempPiece.type=pieceType;
					tempPiece.col=j;
					inTypePiecesOnBoard.push_back(tempPiece);
	
				}
			}
		}

	}

	void computeLegalMoves()
	{
		int row,col,i,type,j;
		piece tempPiece;
		bool isLegal=false;
	
		for (int e=0;e<inTypePiecesOnBoard.size();e++) //for each piece
		{
			//cout <<"Searching for e="<<e;
			row=inTypePiecesOnBoard[e].row;
			col=inTypePiecesOnBoard[e].col;
			type=inTypePiecesOnBoard[e].type;
			isLegal=false;
			for (i=col-1;i>=0;) //check on left
			{
				
				if (input.inCurrentState[row][i]!=-1 && input.inCurrentState[row][i]!=type) //occupied + different type
				{	
					i--;
					isLegal=true;
				}
				else if (input.inCurrentState[row][i]!=-1 && input.inCurrentState[row][i]==type) //occupied + same type
					break;
				else if(input.inCurrentState[row][i]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=row;
					tempPiece.col=i;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[row][i]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
			for (i=col+1;i<8;) //check on right
			{

				if (input.inCurrentState[row][i]!=-1 && input.inCurrentState[row][i]!=type) //occupied + different type
				{
					i++;
					isLegal=true;
				}
				else if (input.inCurrentState[row][i]!=-1 && input.inCurrentState[row][i]==type) //occupied + same type
					break;
				else if(input.inCurrentState[row][i]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=row;
					tempPiece.col=i;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[row][i]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
			for (i=row-1;i>=0;) //check above
			{
				if (input.inCurrentState[i][col]!=-1 && input.inCurrentState[i][col]!=type) //occupied + different type
				{
					i--;
					isLegal=true;
				}
				else if (input.inCurrentState[i][col]!=-1 && input.inCurrentState[i][col]==type) //occupied + same type
					break;
				else if(input.inCurrentState[i][col]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=col;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[i][col]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
			for (i=row+1;i<8;) //check below
			{
				if (input.inCurrentState[i][col]!=-1 && input.inCurrentState[i][col]!=type) //occupied + different type
				{
						i++;
						isLegal=true;
				}
				else if (input.inCurrentState[i][col]!=-1 && input.inCurrentState[i][col]==type) //occupied + same type
					break;
				else if(input.inCurrentState[i][col]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=col;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[i][col]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
			
			i=row-1;
			j=col+1;
			for (;i>=0 && j<8;) //check on right-upper diag
			{

				if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
				{
					i--;
					j++;
					isLegal=true;
				}
				else if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]==type) //occupied + same type
					break;
				else if(input.inCurrentState[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[i][j]==-1 && isLegal==false) 
					break;
			}

			isLegal=false;
			i=row+1;
			j=col-1;
			for (;i<8 && j>=0;) //check on right-lower diag
			{

				if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
				{
					i++;
					j--;
					isLegal=true;
				}
				else if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]==type) //occupied + same type
					break;
				else if(input.inCurrentState[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[i][j]==-1 && isLegal==false)
					break;
			}

			isLegal=false;
			i=row-1;
			j=col-1;
			for (;i>=0 && j>=0;) //check on left-upper diag
			{

				if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
				{
					i--;
					j--;
					isLegal=true;
				}
				else if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]==type) //occupied + same type
					break;
				else if(input.inCurrentState[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[i][j]==-1 && isLegal==false)
					break;
			}

			isLegal=false;
			i=row+1;
			j=col+1;
			for (;i<8 && j<8;) //check on left-lower diag
			{

				if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
				{
					i++;
					j++;
					isLegal=true;
				}
				else if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]==type) //occupied + same type
					break;
				else if(input.inCurrentState[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					legalMovesOnBoard.push_back(tempPiece);
					
					break;
				}
				else if(input.inCurrentState[i][j]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
		}
	}

	void copyArray(int a[][8],int b[][8])
	{
		for (int i=0;i<8;i++)
		{
			for (int j=0;j<8;j++)
			{
				b[i][j]=a[i][j];
			}
		}
	}

	void capturePiecesGreedy(int currentStateOfGame[][8],piece newlyAddedPiece,int playerToMove)
	{
		piece oldPiece,tempPieceForCapture;
		string direction;
		int i,j,row=newlyAddedPiece.row,col=newlyAddedPiece.col,type=playerToMove;
		vector<piece> inTypePiecesOnBoard=initializePieces(currentStateOfGame,playerToMove);
		for(int p=0;p<inTypePiecesOnBoard.size();p++)
		{
			oldPiece=inTypePiecesOnBoard[p];
			direction=findDirection(newlyAddedPiece,oldPiece);
			if (direction=="N")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=row-1;i>oldPiece.row;i--) //check above
				{
					if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=col;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.row)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="S")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=row+1;i<oldPiece.row;i++) //check below
				{
					if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=col;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.row)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="E")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=col+1;i<oldPiece.col;i++) //check right
				{
					if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]!=type) //occupied + different type
					{
						tempPieceForCapture.row=row;
						tempPieceForCapture.col=i;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="W")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=col-1;i>oldPiece.col;i--) //check left
				{
					if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]!=type) //occupied + different type
					{
						tempPieceForCapture.row=row;
						tempPieceForCapture.col=i;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="NE")
			{
				vector<piece> intermediateCapturedPieces;
				i=row-1;
				j=col+1;
				for (;i>oldPiece.row && j<oldPiece.col;) //check on right-upper diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i--;
						j++;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="SW")
			{
				vector<piece> intermediateCapturedPieces;
				i=row+1;
				j=col-1;
				for (;i<oldPiece.row && j>oldPiece.col;) //check on left-lower diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i++;
						j--;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="NW")
			{
				vector<piece> intermediateCapturedPieces;
				i=row-1;
				j=col-1;
				for (;i>oldPiece.row && j>oldPiece.col;) //check on left-upper diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i--;
						j--;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="SE")
			{
				vector<piece> intermediateCapturedPieces;
				i=row+1;
				j=col+1;
				for (;i<oldPiece.row && j<oldPiece.col;) //check on right-lower diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i++;
						j++;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}


		}
	}


	int evaluateGreedyMove(int currentStateOfGame[][8],piece inPiece,int playerToMove)
	{
		int originalStateOfGame[8][8],eval=0;
		copyArray(currentStateOfGame,originalStateOfGame);
		currentStateOfGame[inPiece.row][inPiece.col]=playerToMove;
		capturePiecesGreedy(currentStateOfGame,inPiece,playerToMove);

		for (int i=0;i<8;i++)
		{
			for (int j=0;j<8;j++)
			{
				if (currentStateOfGame[i][j]==input.inAgtPlyrSymbol)
					eval+=inEvaluationMatrix[i][j];
				else if (currentStateOfGame[i][j]!=input.inAgtPlyrSymbol && currentStateOfGame[i][j]!=-1)
					eval-=inEvaluationMatrix[i][j];

			}
		}
		copyArray(originalStateOfGame,currentStateOfGame);
		return eval;
	}

	void nextMove()
	{
		int i,max,outputRow,outputColumn;
		piece temp;
		//ofstream myfile;
		//myfile.open ("output.txt");
		if (legalMovesOnBoard.empty())
		{
			input.indisplay();
		}
		else
		{
			//cout <<"legalMovesOnBoard.size() "<<legalMovesOnBoard.size();
			for(i=0;i<legalMovesOnBoard.size();i++)
			{
				temp=legalMovesOnBoard.at(i);
				temp.evaluation=evaluateGreedyMove(input.inCurrentState,temp,input.inAgtPlyrSymbol);
				BestMovequeue.push(temp);
			}
			temp=BestMovequeue.top();
			BestMovequeue.pop();
			input.inCurrentState[temp.row][temp.col]=input.inAgtPlyrSymbol;
			temp.type=input.inAgtPlyrSymbol;
			BestMovequeue.push(temp);
			capturePieces();
			input.indisplay();
		}
		
	}

	string findDirection(piece source,piece newPiece)
	{
		string direction="";
		if (source.col==newPiece.col && source.row>newPiece.row)
			direction="N";
		else if (source.col==newPiece.col && source.row<newPiece.row)
			direction="S";
		else if (source.row==newPiece.row && source.col>newPiece.col)
			direction="W";
		else if (source.row==newPiece.row && source.col<newPiece.col)
			direction="E";
		else if (source.row>newPiece.row && source.col<newPiece.col)
			direction="NE";
		else if (source.row<newPiece.row && source.col>newPiece.col)
			direction="SW";
		else if (source.row>newPiece.row && source.col>newPiece.col)
			direction="NW";
		else if (source.row<newPiece.row && source.col<newPiece.col)
			direction="SE";
		return direction;
	}

	void capturePieces()
	{
		piece newlyAddedPiece,oldPiece,tempPieceForCapture;
		newlyAddedPiece=BestMovequeue.top();
		BestMovequeue.pop();
		string direction;
		int i,j,row=newlyAddedPiece.row,col=newlyAddedPiece.col,type=newlyAddedPiece.type;
		
		for(int p=0;p<inTypePiecesOnBoard.size();p++)
		{

			oldPiece=inTypePiecesOnBoard[p];
			direction=findDirection(newlyAddedPiece,oldPiece);
			if (direction=="N")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=row-1;i>oldPiece.row;i--) //check above
				{
					if (input.inCurrentState[i][col]!=-1 && input.inCurrentState[i][col]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=col;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.row)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}
			else if (direction=="S")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=row+1;i<oldPiece.row;i++) //check below
				{
					if (input.inCurrentState[i][col]!=-1 && input.inCurrentState[i][col]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=col;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.row)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}
			else if (direction=="E")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=col+1;i<oldPiece.col;i++) //check right
				{
					if (input.inCurrentState[row][i]!=-1 && input.inCurrentState[row][i]!=type) //occupied + different type
					{
						tempPieceForCapture.row=row;
						tempPieceForCapture.col=i;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}
			else if (direction=="W")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=col-1;i>oldPiece.col;i--) //check left
				{
					if (input.inCurrentState[row][i]!=-1 && input.inCurrentState[row][i]!=type) //occupied + different type
					{
						tempPieceForCapture.row=row;
						tempPieceForCapture.col=i;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}
			else if (direction=="NE")
			{
				vector<piece> intermediateCapturedPieces;
				i=row-1;
				j=col+1;
				for (;i>oldPiece.row && j<oldPiece.col;) //check on right-upper diag
				{

					if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i--;
						j++;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}
			else if (direction=="SW")
			{
				vector<piece> intermediateCapturedPieces;
				i=row+1;
				j=col-1;
				for (;i<oldPiece.row && j>oldPiece.col;) //check on left-lower diag
				{

					if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i++;
						j--;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}
			else if (direction=="NW")
			{
				vector<piece> intermediateCapturedPieces;
				i=row-1;
				j=col-1;
				for (;i>oldPiece.row && j>oldPiece.col;) //check on left-upper diag
				{

					if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i--;
						j--;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}
			else if (direction=="SE")
			{
				vector<piece> intermediateCapturedPieces;
				i=row+1;
				j=col+1;
				for (;i<oldPiece.row && j<oldPiece.col;) //check on right-lower diag
				{

					if (input.inCurrentState[i][j]!=-1 && input.inCurrentState[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i++;
						j++;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						input.inCurrentState[tempPieceForCapture.row][tempPieceForCapture.col]=input.inAgtPlyrSymbol;
						
					}
				}
			}


		}
	}

	void playGreedy()
	{
		findPieces(input.inAgtPlyrSymbol);
		computeLegalMoves();
		nextMove();
		//input.indisplay();
	}

	//MiniMax Implementation start

	vector<piece> initializePieces(int currentStateOfGame[][8],int pieceType) //find pieceType type of pieces on board Mini-Max
	{
		//int pieceType=input.inAgtPlyrSymbol;
		vector<piece> tempPiecesVector;
		//cout << "Inside findPieces"<<" piece type "<<pieceType;
		piece tempPiece;
		for (int i=0;i<8;i++)
		{
			for (int j=0;j<8;j++)
			{
				if (currentStateOfGame[i][j]==pieceType)
				{
					tempPiece.row=i;
					tempPiece.type=pieceType;
					tempPiece.col=j;
					tempPiece.weight=inEvaluationMatrix[i][j];
					tempPiecesVector.push_back(tempPiece);
				}

			}
		}
		return tempPiecesVector;

	}

	
	int utility( int state[][8],int playerType)
	{
		int eval=0;
		for (int i=0;i<8;i++)
		{
			for (int j=0;j<8;j++)
			{
				if (state[i][j]==input.inAgtPlyrSymbol)
					eval+=inEvaluationMatrix[i][j];
				else if (state[i][j]!=input.inAgtPlyrSymbol && state[i][j]!=-1)
					eval-=inEvaluationMatrix[i][j];

			}
		}
		return eval;

	}

	vector<piece> removeDuplicatesFromVector(vector<piece> LegalMovesOnBoardMiniMax)
	{
		vector<piece> tempVector;
		piece tempPiece1,tempPiece2;
		bool flag=true;
		for (int i=0;i<LegalMovesOnBoardMiniMax.size();i++)
		{
			tempPiece1=LegalMovesOnBoardMiniMax.at(i);
			for (int j=0;j<tempVector.size();j++)
			{
				tempPiece2=tempVector.at(j);
				if (tempPiece1.row==tempPiece2.row && tempPiece1.col==tempPiece2.col)
				{
					flag=false;
				}
			}
			if (flag==true)
				tempVector.push_back(tempPiece1);
			else
				flag=true;
		}
		return tempVector;
	}

	vector<piece> computeLegalMovesMiniMax(int currentStateOfGame[][8],int playerToMove)
	{
		int row,col,i,type,j;
		piece tempPiece;
		bool isLegal=false;
		//cout <<"Inside computeLegalMoves()";
		vector<piece> LegalMovesOnBoardMiniMax;
		vector<piece> playerToMoveTypePiecesOnBoard=initializePieces(currentStateOfGame,playerToMove);
		for (int e=0;e<playerToMoveTypePiecesOnBoard.size();e++) //for each piece
		{
			//cout <<"Searching for e="<<e;
			row=playerToMoveTypePiecesOnBoard[e].row;
			col=playerToMoveTypePiecesOnBoard[e].col;
			type=playerToMoveTypePiecesOnBoard[e].type;
			isLegal=false;
			for (i=col-1;i>=0;) //check on left
			{
				
				if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]!=type) //occupied + different type
				{	
					i--;
					isLegal=true;
				}
				else if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]==type) //occupied + same type
					break;
				else if(currentStateOfGame[row][i]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=row;
					tempPiece.col=i;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					break;
				}
				else if(currentStateOfGame[row][i]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
			for (i=col+1;i<8;) //check on right
			{

				if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]!=type) //occupied + different type
				{
					i++;
					isLegal=true;
				}
				else if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]==type) //occupied + same type
					break;
				else if(currentStateOfGame[row][i]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=row;
					tempPiece.col=i;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					isLegal=false;
					break;
				}
				else if(currentStateOfGame[row][i]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
			for (i=row-1;i>=0;) //check above
			{
				if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]!=type) //occupied + different type
				{
					i--;
					isLegal=true;
				}
				else if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]==type) //occupied + same type
					break;
				else if(currentStateOfGame[i][col]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=col;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					isLegal=false;
					break;
				}
				else if(currentStateOfGame[i][col]==-1 && isLegal==false)
					break;
			}
			isLegal=false;
			for (i=row+1;i<8;) //check below
			{
				if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]!=type) //occupied + different type
				{
						i++;
						isLegal=true;
				}
				else if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]==type) //occupied + same type
					break;
				else if(currentStateOfGame[i][col]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=col;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					isLegal=false;
					break;
				}
				else if(currentStateOfGame[i][col]==-1 && isLegal==false)
					break;
			}

			isLegal=false;
			i=row-1;
			j=col+1;
			for (;i>=0 && j<8;) //check on right-upper diag
			{

				if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
				{
					i--;
					j++;
					isLegal=true;
				}
				else if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]==type) //occupied + same type
					break;
				else if(currentStateOfGame[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					isLegal=false;
					break;
				}
				else if(currentStateOfGame[i][j]==-1 && isLegal==false) 
					break;
			}

			isLegal=false;
			i=row+1;
			j=col-1;
			for (;i<8 && j>=0;) //check on right-lower diag
			{

				if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
				{
					i++;
					j--;
					isLegal=true;
				}
				else if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]==type) //occupied + same type
					break;
				else if(currentStateOfGame[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					isLegal=false;
					break;
				}
				else if(currentStateOfGame[i][j]==-1 && isLegal==false)
					break;
			}

			isLegal=false;
			i=row-1;
			j=col-1;
			for (;i>=0 && j>=0;) //check on left-upper diag
			{

				if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
				{
					i--;
					j--;
					isLegal=true;
				}
				else if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]==type) //occupied + same type
					break;
				else if(currentStateOfGame[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					isLegal=false;
					break;
				}
				else if(currentStateOfGame[i][j]==-1 && isLegal==false)
					break;
			}

			isLegal=false;
			i=row+1;
			j=col+1;
			for (;i<8 && j<8;) //check on left-lower diag
			{

				if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
				{
					i++;
					j++;
					isLegal=true;
				}
				else if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]==type) //occupied + same type
					break;
				else if(currentStateOfGame[i][j]==-1 && isLegal==true) //unoccupied + legal
				{
					tempPiece.row=i;
					tempPiece.col=j;
					tempPiece.type=-1;
					tempPiece.srcCol=col;
					tempPiece.srcRow=row;
					LegalMovesOnBoardMiniMax.push_back(tempPiece);
					isLegal=false;
					break;
				}
				else if(currentStateOfGame[i][j]==-1 && isLegal==false)
					break;
			}


			

		}
		//cout <<"legalMovesOnBoard found are :"<<legalMovesOnBoard.size();
		return removeDuplicatesFromVector(LegalMovesOnBoardMiniMax);
	}

	char getSymbol(int input)
	{
		if (input==0)
			return 'O';
		else if (input==1)
			return 'X';
		else 
			return '*';
	}

	void finalOutputMM(int currentStateOfGame[][8])
	{
		ofstream myfile;
		myfile.open("output.txt");
			for (int i = 0; i < 8; i++)  //display output
			{
				for (int j = 0; j < 8; j++)
				{
					myfile<<getSymbol(currentStateOfGame[i][j]);
						
				}
				if(i!=7)
					myfile<<"\n";
			
			}
			ifstream logfile;
			logfile.open("logoutput.txt");
			string tempstr="";
			while(!logfile.eof())
			{
				logfile>>tempstr;
				if(tempstr!="")
				{
					myfile<<"\n";
					myfile<<tempstr;
				}
				tempstr="";
			}
			myfile.close();
			logfile.close();
	}

	void MiniMaxDecision(int currentStateOfGame[][8],int playerToMove,int opponentPlayer)
	{
		int depth=0,val;
		ofstream myfile;
		
		vector<piece> miniMaxLegalMovesOnBoard,tempVector,opponentTempVector;
		piece temp,newlyAddedPiece,moveThisPiece;
		int copyCurrentStateOfGame[8][8],max=INT_MIN;
		priority_queue<piece, vector<piece>, sortMoves> tempMovequeue;

		myfile.open("logoutput.txt");
		myfile<<"Node,Depth,Value"<<"\n";
		myfile<<"root,"<<NumtoString(depth)<<","<<NumtoString(max)<<"\n";			
		myfile.close();

		if (explicitTerminal(currentStateOfGame,playerToMove,opponentPlayer))
		{
			myfile.open("logoutput.txt");
			myfile<<"Node,Depth,Value"<<"\n";
			int utilityVal=utility(currentStateOfGame,playerToMove);
			myfile<<"root,"<<0<<","<<NumtoString(utilityVal)<<"\n";			
			myfile.close();
			finalOutputMM(currentStateOfGame);
		}


else{ // !explicitTerminal
		tempVector=computeLegalMovesMiniMax(currentStateOfGame,playerToMove);

		if(tempVector.empty())
		{
				piece PassPiece;
				PassPiece.row=-100;
				PassPiece.col=-100;
				PassPiece.evaluation=INT_MAX;
				depth=depth+1;
				val=playMinMove(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove);
				if (val>max)
					max=val;
							
				printLog("root",0,NumtoString(max));
				finalOutputMM(currentStateOfGame);
			
		}
		else
		{
			for (int i=0;i<tempVector.size();i++) //sort moves in order we want to traverse i.e row-wise and then column-wise
			{
				temp=tempVector.at(i);
				temp.evaluation=INT_MAX;
				tempMovequeue.push(temp);
			}
			while(!tempMovequeue.empty()) //sort moves in order we want to traverse i.e row-wise and then column-wise
			{
				temp=tempMovequeue.top();
				tempMovequeue.pop();
				miniMaxLegalMovesOnBoard.push_back(temp);
			}
			//Computed Legal moves in-order
		
			depth=depth+1;
		
			copyArray(currentStateOfGame,copyCurrentStateOfGame); //save original state
			for (int i=0;i<miniMaxLegalMovesOnBoard.size();i++)  //d3, c4, f5, e6
			{
				currentStateOfGame[miniMaxLegalMovesOnBoard[i].row][miniMaxLegalMovesOnBoard[i].col]=playerToMove;
				newlyAddedPiece=miniMaxLegalMovesOnBoard.at(i);
				capturePiecesMiniMax(currentStateOfGame,newlyAddedPiece,playerToMove);
				
				val=playMinMove(currentStateOfGame,newlyAddedPiece,opponentPlayer,depth,playerToMove);
				
				if (val>max)
				{
					max=val;
					moveThisPiece.row=newlyAddedPiece.row;
					moveThisPiece.col=newlyAddedPiece.col;
					moveThisPiece.type=playerToMove;	
				}
				printLog("root",0,NumtoString(max));
				copyArray(copyCurrentStateOfGame,currentStateOfGame);
			}
			currentStateOfGame[moveThisPiece.row][moveThisPiece.col]=playerToMove;
			capturePiecesMiniMax(currentStateOfGame,moveThisPiece,playerToMove);
			finalOutputMM(currentStateOfGame);
		}
}
 
}

	void printLog(string nodeName,int depth,string evaluation)
	{
		ofstream myfile;
		myfile.open("logoutput.txt",fstream::ate|fstream::out|fstream::in);
		myfile<<nodeName<<","<<depth<<","<<evaluation<<"\n";
		myfile.close();
	}

	string getNodeName(int row,int col)
	{
		string columns[8]={"a","b","c","d","e","f","g","h"};
		string rows[8]={"1","2","3","4","5","6","7","8"};
		if((row==-99 && col==-99) || (row==-100 && col==-100) || (row==-200 && col==-200))
			return("pass");
		else
			return(columns[col]+rows[row]);
	}

	void printLogAB(string nodeName,int depth,string evaluation,string alpha,string beta)
	{
		ofstream myfile;
		myfile.open("logoutput.txt",fstream::ate|fstream::out|fstream::in);
		myfile<<nodeName<<","<<depth<<","<<evaluation<<","<<alpha<<","<<beta<<"\n";
		myfile.close();
	}

	int playMaxMove(int currentStateOfGame[][8],piece parentAddedPiece,int playerToMove,int depth,int opponentPlayer)
	{
		vector<piece> miniMaxLegalMovesOnBoard,tempVector,opponentTempVector;
		piece temp,newlyAddedPiece;
		int val,max=INT_MIN,utilityVal,num=1;
		int copyCurrentStateOfGame[8][8];
		priority_queue<piece, vector<piece>, sortMoves> tempMovequeue;
		ofstream myfile;
	
		if (depth==input.inCutOffDepth)
		{
			utilityVal=utility(currentStateOfGame,playerToMove);
			printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(utilityVal));
			return utilityVal;
		}
		else
		{
			if (explicitTerminal(currentStateOfGame,playerToMove,opponentPlayer))
			{
				int utilityVal=utility(currentStateOfGame,playerToMove);
				if(utilityVal>max)
					max=utilityVal;
				printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(max));
				return max;
			}
			depth=depth+1;
			tempVector=computeLegalMovesMiniMax(currentStateOfGame,playerToMove);
			if(tempVector.empty())
			{
				if ((parentAddedPiece.row!=-100 && parentAddedPiece.col!=-100) && (parentAddedPiece.row!=-200 && parentAddedPiece.col!=-200))
				{
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max));
					piece PassPiece;
					PassPiece.row=-100;
					PassPiece.col=-100;
					PassPiece.evaluation=INT_MAX;

					val=playMinMove(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove);
					if(val>max)
						max=val;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max));
					return max;
				}
				else if (parentAddedPiece.row==-100 && parentAddedPiece.col==-100)
				{
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation));
					piece PassPiece;
					PassPiece.row=-200;
					PassPiece.col=-200;
					PassPiece.evaluation=INT_MAX;

					val=playMinMove(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove);
					if(val>max)
						max=val;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max));
					return max;
				}

				else if(parentAddedPiece.row==-200 && parentAddedPiece.col==-200)
				{
					int utilitiVal=utility(currentStateOfGame,playerToMove);
					if (max<utilitiVal)
						max=utilitiVal;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max));
					return max;
				}
			}
			else
			{
				printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation));
				for (int i=0;i<tempVector.size();i++) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempVector.at(i);
					temp.evaluation=INT_MAX;
					tempMovequeue.push(temp);
				}
				while(!tempMovequeue.empty()) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempMovequeue.top();
					tempMovequeue.pop();
					miniMaxLegalMovesOnBoard.push_back(temp);
				}
				copyArray(currentStateOfGame,copyCurrentStateOfGame);
				
				for (int i=0;i<miniMaxLegalMovesOnBoard.size();i++) 
				{
					//copyArray(copyCurrentStateOfGame,currentStateOfGame);
					currentStateOfGame[miniMaxLegalMovesOnBoard[i].row][miniMaxLegalMovesOnBoard[i].col]=playerToMove;
					newlyAddedPiece=miniMaxLegalMovesOnBoard.at(i);
					capturePiecesMiniMax(currentStateOfGame,newlyAddedPiece,playerToMove);
		
					val=playMinMove(currentStateOfGame,newlyAddedPiece,opponentPlayer,depth,playerToMove);
					if (val>parentAddedPiece.evaluation)
					{
						parentAddedPiece.evaluation=val;
					}
					//cout<<getNodeName(miniMaxLegalMovesOnBoard[i].row,miniMaxLegalMovesOnBoard[i].col)<<","<<depth<<","<<miniMaxLegalMovesOnBoard[i].evaluation<<"\n";
					if (val>max)
						max=val;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max));
					
					copyArray(copyCurrentStateOfGame,currentStateOfGame);
				}
				return max;
			}
		}
	}

	int playMinMove(int currentStateOfGame[][8],piece parentAddedPiece,int playerToMove,int depth,int opponentPlayer)
	{
		vector<piece> miniMaxLegalMovesOnBoard,tempVector,opponentTempVector;
		piece temp, newlyAddedPiece;
		int val,min=INT_MAX,utilityVal;
		int copyCurrentStateOfGame[8][8];
		priority_queue<piece, vector<piece>, sortMoves> tempMovequeue;
		ofstream myfile;
		
		if (depth==input.inCutOffDepth)
		{
			utilityVal=utility(currentStateOfGame,playerToMove);
			printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(utilityVal));
			return utilityVal;
		}
		else
		{
			if (explicitTerminal(currentStateOfGame,playerToMove,opponentPlayer))
			{
				int utilityVal=utility(currentStateOfGame,playerToMove);
				if(utilityVal<min)
					min=utilityVal;
				printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(min));
				return min;
			}
			depth=depth+1;
			tempVector=computeLegalMovesMiniMax(currentStateOfGame,playerToMove);
			if(tempVector.empty())
			{

				if ((parentAddedPiece.row!=-100 && parentAddedPiece.col!=-100) && (parentAddedPiece.row!=-200 && parentAddedPiece.col!=-200))
				{
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation));
					piece PassPiece;
					PassPiece.row=-100;
					PassPiece.col=-100;
					PassPiece.evaluation=INT_MIN;

					val=playMaxMove(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove);
					if (min>val)
						min=val;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min));
					return min;
				}
				else if (parentAddedPiece.row==-100 && parentAddedPiece.col==-100)
				{
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation));
					piece PassPiece;
					PassPiece.row=-200;
					PassPiece.col=-200;
					PassPiece.evaluation=INT_MIN;
					val=playMaxMove(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove);
					if (min>val)
						min=val;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min));
					return min;
				}
				else if (parentAddedPiece.row==-200 && parentAddedPiece.col==-200)
				{
					int utilitiVal=utility(currentStateOfGame,playerToMove);
					if (min>utilitiVal)
						min=utilitiVal;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min));
					return min;
				}
				
			}
			else
			{
				printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation));
				for (int i=0;i<tempVector.size();i++) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempVector.at(i);
					temp.evaluation=INT_MIN;
					tempMovequeue.push(temp);
				}
				while(!tempMovequeue.empty()) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempMovequeue.top();
					tempMovequeue.pop();
					miniMaxLegalMovesOnBoard.push_back(temp);
				}

				copyArray(currentStateOfGame,copyCurrentStateOfGame);
				for (int i=0;i<miniMaxLegalMovesOnBoard.size();i++) //c3,e3,c5
				{
				
					currentStateOfGame[miniMaxLegalMovesOnBoard[i].row][miniMaxLegalMovesOnBoard[i].col]=playerToMove;
					newlyAddedPiece=miniMaxLegalMovesOnBoard.at(i);
					capturePiecesMiniMax(currentStateOfGame,newlyAddedPiece,playerToMove);
									
					val=playMaxMove(currentStateOfGame,newlyAddedPiece,opponentPlayer,depth,playerToMove);
					if (val<parentAddedPiece.evaluation)
					{
						parentAddedPiece.evaluation=val;
					}
					//cout<<getNodeName(miniMaxLegalMovesOnBoard[i].row,miniMaxLegalMovesOnBoard[i].col)<<","<<depth<<","<<miniMaxLegalMovesOnBoard[i].evaluation<<"\n";
					if (val<min)
						min=val;
				
					//return min;
					printLog(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min));
					copyArray(copyCurrentStateOfGame,currentStateOfGame);
				}
			return min;
			}
		}
	}

	void capturePiecesMiniMax(int currentStateOfGame[][8],piece newlyAddedPiece,int playerToMove)
	{
		piece oldPiece,tempPieceForCapture;
		string direction;
		int i,j,row=newlyAddedPiece.row,col=newlyAddedPiece.col,type=playerToMove;
		vector<piece> inTypePiecesOnBoard=initializePieces(currentStateOfGame,playerToMove);
		for(int p=0;p<inTypePiecesOnBoard.size();p++)
		{
			oldPiece=inTypePiecesOnBoard[p];
			direction=findDirection(newlyAddedPiece,oldPiece);
			if (direction=="N")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=row-1;i>oldPiece.row;i--) //check above
				{
					if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=col;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.row)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="S")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=row+1;i<oldPiece.row;i++) //check below
				{
					if (currentStateOfGame[i][col]!=-1 && currentStateOfGame[i][col]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=col;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.row)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="E")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=col+1;i<oldPiece.col;i++) //check right
				{
					if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]!=type) //occupied + different type
					{
						tempPieceForCapture.row=row;
						tempPieceForCapture.col=i;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="W")
			{
				vector<piece> intermediateCapturedPieces;
				for (i=col-1;i>oldPiece.col;i--) //check left
				{
					if (currentStateOfGame[row][i]!=-1 && currentStateOfGame[row][i]!=type) //occupied + different type
					{
						tempPieceForCapture.row=row;
						tempPieceForCapture.col=i;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
					}
					else
						break;
				}
				if (i==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="NE")
			{
				vector<piece> intermediateCapturedPieces;
				i=row-1;
				j=col+1;
				for (;i>oldPiece.row && j<oldPiece.col;) //check on right-upper diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i--;
						j++;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="SW")
			{
				vector<piece> intermediateCapturedPieces;
				i=row+1;
				j=col-1;
				for (;i<oldPiece.row && j>oldPiece.col;) //check on left-lower diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i++;
						j--;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="NW")
			{
				vector<piece> intermediateCapturedPieces;
				i=row-1;
				j=col-1;
				for (;i>oldPiece.row && j>oldPiece.col;) //check on left-upper diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i--;
						j--;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}
			else if (direction=="SE")
			{
				vector<piece> intermediateCapturedPieces;
				i=row+1;
				j=col+1;
				for (;i<oldPiece.row && j<oldPiece.col;) //check on right-lower diag
				{

					if (currentStateOfGame[i][j]!=-1 && currentStateOfGame[i][j]!=type) //occupied + different type
					{
						tempPieceForCapture.row=i;
						tempPieceForCapture.col=j;
						intermediateCapturedPieces.push_back(tempPieceForCapture);
						i++;
						j++;
					}
					else 
						break;
					
				}
				if (i==oldPiece.row && j==oldPiece.col)
				{
					for (int z=0;z<intermediateCapturedPieces.size();z++)
					{
						tempPieceForCapture=intermediateCapturedPieces.at(z);
						currentStateOfGame[tempPieceForCapture.row][tempPieceForCapture.col]=playerToMove;
						
					}
				}
			}


		}
	}

	//Alpha-Beta Algorithm Start

	void finalOutputAB(int currentStateOfGame[][8])
	{
		ofstream myfile; 
		myfile.open("output.txt");
			for (int i = 0; i < 8; i++)  //display output
			{
				for (int j = 0; j < 8; j++)
				{
					myfile<<getSymbol(currentStateOfGame[i][j]);	
				}
				if(i!=7)
					myfile<<"\n";
			
			}
			ifstream logfile;
			logfile.open("logoutput.txt");
			string tempstr="";
			while(!logfile.eof())
			{
				logfile>>tempstr;
				if(tempstr!="")
				{
					myfile<<"\n";
					myfile<<tempstr;
				}
				tempstr="";
			}
			myfile.close();
			logfile.close();
	}

	bool explicitTerminal(int currentStateOfGame[][8],int playerToMove,int opponentPlayer)
	{
		vector<piece> playerToMovePieces,opponentPlayerPieces;
		playerToMovePieces=initializePieces(currentStateOfGame,playerToMove);
		opponentPlayerPieces=initializePieces(currentStateOfGame,opponentPlayer);

		if (playerToMovePieces.size()==0 || opponentPlayerPieces.size()==0)
			return true;
		return false;
	}
	
	void AlphaBetaDecision(int currentStateOfGame[][8],int playerToMove,int opponentPlayer)
	{
		int depth=0,val;
		int tval;
		
		vector<piece> alphaBetaLegalMovesOnBoard,tempVector,opponentTempVector;
		piece temp,newlyAddedPiece,moveThisPiece;
		int copyCurrentStateOfGame[8][8],max=INT_MIN,alpha=INT_MIN,beta=INT_MAX;
		priority_queue<piece, vector<piece>, sortMoves> tempMovequeue;
		ofstream myfile;

		myfile.open("logoutput.txt");
		myfile<<"Node,Depth,Value,Alpha,Beta"<<"\n";
		myfile<<"root,"<<NumtoString(depth)<<","<<NumtoString(max)<<","<<NumtoString(alpha)<<","<<NumtoString(beta)<<"\n";			
		myfile.close();

		if (explicitTerminal(currentStateOfGame,playerToMove,opponentPlayer))
		{
			myfile.open("logoutput.txt");
			myfile<<"Node,Depth,Value,Alpha,Beta"<<"\n";
			int utilityVal=utility(currentStateOfGame,playerToMove);
			myfile<<"root,"<<0<<","<<NumtoString(utilityVal)<<","<<NumtoString(alpha)<<","<<NumtoString(beta)<<"\n";			
			myfile.close();
			finalOutputAB(currentStateOfGame);
		}
		
else{ // !explicitTerminal
		tempVector=computeLegalMovesMiniMax(currentStateOfGame,playerToMove);
		if(tempVector.empty())
		{
			piece PassPiece;
			PassPiece.row=-100; //pass1
			PassPiece.col=-100; //pass1
			PassPiece.evaluation=INT_MAX;
			depth=depth+1;
			val=playMinMoveAlphaBeta(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove,alpha,beta);
			if (val>max)
				max=val;
			if (val>alpha)
				alpha=val;
			printLogAB("root",0,NumtoString(max),NumtoString(alpha),NumtoString(beta));		
			finalOutputAB(currentStateOfGame);

		}
		else
		{
			for (int i=0;i<tempVector.size();i++) //sort moves in order we want to traverse i.e row-wise and then column-wise
			{
				temp=tempVector.at(i);
				temp.evaluation=INT_MAX;
				tempMovequeue.push(temp);
			}
			while(!tempMovequeue.empty()) //sort moves in order we want to traverse i.e row-wise and then column-wise
			{
				temp=tempMovequeue.top();
				tempMovequeue.pop();
				alphaBetaLegalMovesOnBoard.push_back(temp);
			}
			//Computed Legal moves in-order
		
			depth=depth+1;
			copyArray(currentStateOfGame,copyCurrentStateOfGame); //save original state
			for (int i=0;i<alphaBetaLegalMovesOnBoard.size();i++)  //d3, c4, f5, e6
			{
				currentStateOfGame[alphaBetaLegalMovesOnBoard[i].row][alphaBetaLegalMovesOnBoard[i].col]=playerToMove;
				newlyAddedPiece=alphaBetaLegalMovesOnBoard.at(i);
				capturePiecesMiniMax(currentStateOfGame,newlyAddedPiece,playerToMove);

				val=playMinMoveAlphaBeta(currentStateOfGame,newlyAddedPiece,opponentPlayer,depth,playerToMove,alpha,beta);
				
				if (val>max)
				{
					max=val;
					moveThisPiece.row=newlyAddedPiece.row;
					moveThisPiece.col=newlyAddedPiece.col;
					moveThisPiece.type=playerToMove;	
				}
				if (val>alpha)
					alpha=val;
				printLogAB("root",0,NumtoString(max),NumtoString(alpha),NumtoString(beta));		
				copyArray(copyCurrentStateOfGame,currentStateOfGame);
			}
			currentStateOfGame[moveThisPiece.row][moveThisPiece.col]=playerToMove;
			capturePiecesMiniMax(currentStateOfGame,moveThisPiece,playerToMove);
			finalOutputAB(currentStateOfGame);
		}//new added else
}		
}

	int playMaxMoveAlphaBeta(int currentStateOfGame[][8],piece parentAddedPiece,int playerToMove,int depth,int opponentPlayer,int alpha,int beta)
	{
		vector<piece> alphaBetaLegalMovesOnBoard,tempVector,opponentTempVector;
		piece temp,newlyAddedPiece;
		int val,max=INT_MIN,utilityVal;
		int copyCurrentStateOfGame[8][8];
		priority_queue<piece, vector<piece>, sortMoves> tempMovequeue;
		ofstream myfile;
		
		if (depth==input.inCutOffDepth)
		{
			utilityVal=utility(currentStateOfGame,playerToMove);
			printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(utilityVal),NumtoString(alpha),NumtoString(beta));
			return utilityVal;
		}
		else
		{
			
			if (explicitTerminal(currentStateOfGame,playerToMove,opponentPlayer))
			{
				int utilityVal=utility(currentStateOfGame,playerToMove);
				if(utilityVal>max)
					max=utilityVal;
				printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(max),NumtoString(alpha),NumtoString(beta));
				return max;
			}
			depth=depth+1;
			tempVector=computeLegalMovesMiniMax(currentStateOfGame,playerToMove);
			if (tempVector.empty())
			{
				if ((parentAddedPiece.row!=-100 && parentAddedPiece.col!=-100) && (parentAddedPiece.row!=-200 && parentAddedPiece.col!=-200))
				{
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));
					piece PassPiece;
					PassPiece.row=-100;
					PassPiece.col=-100;
					PassPiece.evaluation=INT_MAX;

					val=playMinMoveAlphaBeta(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove,alpha,beta);
					if(val>max)
					{
						max=val;
						
						if (alpha<max)
						{
							if (max>=beta)
							{
								printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));
								return beta;
							}
							alpha=max;
						}
						
					}
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));
					return max;
				}
				else if (parentAddedPiece.row==-100 && parentAddedPiece.col==-100)
				{
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation),NumtoString(alpha),NumtoString(beta));
					piece PassPiece;
					PassPiece.row=-200;
					PassPiece.col=-200;
					PassPiece.evaluation=INT_MAX;

					val=playMinMoveAlphaBeta(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove,alpha,beta);
					if(val>max)
					{
						max=val;
						
						if (alpha<max)
						{
							if (max>=beta)
							{
								printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));
								return beta;
							}
							alpha=max;
						}
						
					}
					
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));
				
					return max;
				}

				else if(parentAddedPiece.row==-200 && parentAddedPiece.col==-200)
				{
					int utilitiVal=utility(currentStateOfGame,playerToMove);
					if (max<utilitiVal)
						max=utilitiVal;
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));
					return max;
				}
				
			}
			else
			{
				
				printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation),NumtoString(alpha),NumtoString(beta));
				for (int i=0;i<tempVector.size();i++) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempVector.at(i);
					temp.evaluation=INT_MAX;
					tempMovequeue.push(temp);
				}
				while(!tempMovequeue.empty()) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempMovequeue.top();
					tempMovequeue.pop();
					alphaBetaLegalMovesOnBoard.push_back(temp);
				}
				copyArray(currentStateOfGame,copyCurrentStateOfGame);
				for (int i=0;i<alphaBetaLegalMovesOnBoard.size();i++) 
				{
					
					currentStateOfGame[alphaBetaLegalMovesOnBoard[i].row][alphaBetaLegalMovesOnBoard[i].col]=playerToMove;
					newlyAddedPiece=alphaBetaLegalMovesOnBoard.at(i);
					capturePiecesMiniMax(currentStateOfGame,newlyAddedPiece,playerToMove);
					
					val=playMinMoveAlphaBeta(currentStateOfGame,newlyAddedPiece,opponentPlayer,depth,playerToMove,alpha,beta);
					if (val>parentAddedPiece.evaluation)
					{
						parentAddedPiece.evaluation=val;
					}
				
					if(val>max)
					{
						max=val;
						
						if (alpha<max)
						{
							if (max>=beta)
							{
								printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));
								return beta;
							}
							alpha=max;
						}
						
					}
					
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(max),NumtoString(alpha),NumtoString(beta));


					copyArray(copyCurrentStateOfGame,currentStateOfGame);
				}
				if(getNodeName(parentAddedPiece.row,parentAddedPiece.col)=="pass")
					return max;
				return alpha;
			}
		}
	}

	int playMinMoveAlphaBeta(int currentStateOfGame[][8],piece parentAddedPiece,int playerToMove,int depth,int opponentPlayer,int alpha,int beta)
	{
		vector<piece> alphaBetaLegalMovesOnBoard,tempVector,opponentTempVector;
		piece temp, newlyAddedPiece;
		int val,min=INT_MAX,utilityVal;
		int copyCurrentStateOfGame[8][8];
		priority_queue<piece, vector<piece>, sortMoves> tempMovequeue;
		ofstream myfile;
		
		if (depth==input.inCutOffDepth)
		{
			utilityVal=utility(currentStateOfGame,playerToMove);
			printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(utilityVal),NumtoString(alpha),NumtoString(beta));
			return utilityVal;
		}
		else
		{
			if (explicitTerminal(currentStateOfGame,playerToMove,opponentPlayer))
			{
				int utilityVal=utility(currentStateOfGame,playerToMove);
				if(utilityVal<min)
					min=utilityVal;
				printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth,NumtoString(min),NumtoString(alpha),NumtoString(beta));
				return min;
			}
			depth=depth+1;
			tempVector=computeLegalMovesMiniMax(currentStateOfGame,playerToMove);
			if (tempVector.empty())
			{
				if ((parentAddedPiece.row!=-100 && parentAddedPiece.col!=-100) && (parentAddedPiece.row!=-200 && parentAddedPiece.col!=-200))
				{
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation),NumtoString(alpha),NumtoString(beta));
					piece PassPiece;
					PassPiece.row=-100;
					PassPiece.col=-100;
					PassPiece.evaluation=INT_MIN;
				
					val=playMaxMoveAlphaBeta(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove,alpha,beta);
				
					if (val<min)
						{
							min=val;

							if (beta>val)
							{
								if (min<=alpha)
								{
									printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min),NumtoString(alpha),NumtoString(beta));
									return alpha;
								}
								beta=min;
							}
						}
					
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min),NumtoString(alpha),NumtoString(beta));
					return min;
				}
				else if (parentAddedPiece.row==-100 && parentAddedPiece.col==-100)
				{
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation),NumtoString(alpha),NumtoString(beta));
					piece PassPiece;
					PassPiece.row=-200;
					PassPiece.col=-200;
					PassPiece.evaluation=INT_MIN;
				
					val=playMaxMoveAlphaBeta(currentStateOfGame,PassPiece,opponentPlayer,depth,playerToMove,alpha,beta);
				
					if (val<min)
						{
							min=val;

							if (beta>val)
							{
								if (min<=alpha)
								{
									printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min),NumtoString(alpha),NumtoString(beta));
									return alpha;
								}
								beta=min;
							}
						}
					
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min),NumtoString(alpha),NumtoString(beta));
					return min;
				}
				else if (parentAddedPiece.row==-200 && parentAddedPiece.col==-200)
				{
					int utilitiVal=utility(currentStateOfGame,playerToMove);
					if (min>utilitiVal)
						min=utilitiVal;
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min),NumtoString(alpha),NumtoString(beta));
					return min;
				}

			}
			else
			{
				printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(parentAddedPiece.evaluation),NumtoString(alpha),NumtoString(beta));
				for (int i=0;i<tempVector.size();i++) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempVector.at(i);
					temp.evaluation=INT_MIN;
					tempMovequeue.push(temp);
				}
				while(!tempMovequeue.empty()) //sort moves in order we want to traverse i.e row-wise and then column-wise
				{
					temp=tempMovequeue.top();
					tempMovequeue.pop();
					alphaBetaLegalMovesOnBoard.push_back(temp);
				}
				
				copyArray(currentStateOfGame,copyCurrentStateOfGame);
				for (int i=0;i<alphaBetaLegalMovesOnBoard.size();i++) //c3,e3,c5
				{
					currentStateOfGame[alphaBetaLegalMovesOnBoard[i].row][alphaBetaLegalMovesOnBoard[i].col]=playerToMove;
					newlyAddedPiece=alphaBetaLegalMovesOnBoard.at(i);
					capturePiecesMiniMax(currentStateOfGame,newlyAddedPiece,playerToMove);
				
					val=playMaxMoveAlphaBeta(currentStateOfGame,newlyAddedPiece,opponentPlayer,depth,playerToMove,alpha,beta);
					if (val<parentAddedPiece.evaluation)
					{
						parentAddedPiece.evaluation=val;
					}

					if (val<min)
					{
						min=val;

						if (beta>val)
						{
							if (min<=alpha)
							{
								printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min),NumtoString(alpha),NumtoString(beta));
								return alpha;
							}
							beta=min;
					
						}
					}
					
					printLogAB(getNodeName(parentAddedPiece.row,parentAddedPiece.col),depth-1,NumtoString(min),NumtoString(alpha),NumtoString(beta));

					copyArray(copyCurrentStateOfGame,currentStateOfGame);
				}
				if(getNodeName(parentAddedPiece.row,parentAddedPiece.col)=="pass")
					return min;
				return beta;
			}			
		}
	}
};


int main() {
	GamePlay game;
	game.input.initialize();
	remove("output.txt");
	if (game.input.inAlgoType==1)
	{
		game.playGreedy();
	}
	else if (game.input.inAlgoType==2)
	{
		game.MiniMaxDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		remove("logoutput.txt");
		
	}
	else if (game.input.inAlgoType==3)
	{
		game.AlphaBetaDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		remove("logoutput.txt");
		
	}
	
return 0;
}


/*int main() {
	GamePlay game;
	game.input.initialize();
	remove("output.txt");
	int originalGame[8][8];
	game.copyArray(game.input.inCurrentState,originalGame);
	
		game.input.inAlgoType=1;

		game.input.inCutOffDepth=1;
		char file[]="greedy_1.txt";
		game.playGreedy();
		rename("output.txt",file);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=2;
		char file2[]="greedy_2.txt";
		game.playGreedy();
		rename("output.txt",file2);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=3;
		char file3[]="greedy_3.txt";
		game.playGreedy();
		rename("output.txt",file3);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=4;
		char file4[]="greedy_4.txt";
		game.playGreedy();
		rename("output.txt",file4);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=5;
		char file5[]="greedy_5.txt";
		game.playGreedy();
		rename("output.txt",file5);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=6;
		char file6[]="greedy_6.txt";
		game.playGreedy();
		rename("output.txt",file6);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inAlgoType=2;

		game.input.inCutOffDepth=1;
		char file11[]="minmax_1.txt";
		game.MiniMaxDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file11);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=2;
		char file12[]="minmax_2.txt";
		game.MiniMaxDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file12);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=3;
		char file13[]="minmax_3.txt";
		game.MiniMaxDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file13);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=4;
		char file14[]="minmax_4.txt";
		game.MiniMaxDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file14);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=5;
		char file15[]="minmax_5.txt";
		game.MiniMaxDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file15);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=6;
		char file16[]="minmax_6.txt";
		game.MiniMaxDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file16);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inAlgoType=3;

		game.input.inCutOffDepth=1;
		char file21[]="alphaBeta_1.txt";
		game.AlphaBetaDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file21);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=2;
		char file22[]="alphaBeta_2.txt";
		game.AlphaBetaDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file22);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=3;
		char file23[]="alphaBeta_3.txt";
		game.AlphaBetaDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file23);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=4;
		char file24[]="alphaBeta_4.txt";
		game.AlphaBetaDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file24);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=5;
		char file25[]="alphaBeta_5.txt";
		game.AlphaBetaDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file25);
		game.copyArray(originalGame,game.input.inCurrentState);
		game.input.inCutOffDepth=6;
		char file26[]="alphaBeta_6.txt";
		game.AlphaBetaDecision(game.input.inCurrentState,game.input.inAgtPlyrSymbol,game.input.inOtherPlyrSymbol);
		rename("output.txt",file26);

return 0;
}
*/

