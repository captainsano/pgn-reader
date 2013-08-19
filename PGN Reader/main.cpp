//
//  main.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 10/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include <iostream>
#include "PGNFile.h"

#define __TEST_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/")

/*

int main(int argc, const char * argv[]) {
	PGNFile f(__TEST_FILE_PATH__ + "1.pgn");
	
	std::cout << f.getGame(5);
	
	std::cout << std::endl;
}

*/

enum TokenType {
	TokenInvalid,
	TokenWhiteSpace,
	TokenDot,
	TokenGameTermination,
	TokenNAG,
	TokenTextAnnotation,
	TokenVariationBegin,
	TokenVariationEnd,
	TokenMoveNumber,
	// Move Tokens
	TokenGenericMove,
};

enum TokenSubType {
	TokenSubTypeNone,
	// Move Sub-Types
	TokenSubTypeMoveNullMove,
	TokenSubTypeMovePawn,
	TokenSubTypeMovePawnCapture,
	TokenSubTypeMovePawnPromotion,
	TokenSubTypeMovePawnCapturePromotion,
	TokenSubTypeMovePiece,
	TokenSubTypeMovePieceFromFile,
	TokenSubTypeMovePieceFromRank,
	TokenSubTypeMovePieceFromSquare,
	TokenSubTypeMoveQueenSideCastling,
	TokenSubTypeMoveKingSideCastling,
};

struct Token {
	TokenType		type	= TokenInvalid;
	TokenSubType	subType	= TokenSubTypeNone;			// Used only for moves
	std::string		contents = "";
	unsigned int	charactersConsumed = 0;
	
	Token() = default;
};

Token nextToken(std::string::const_iterator begin, std::string::const_iterator end);

int main() {
	std::string myString =
	"1. "
	"Nf3 Nf6 2. c4 c5 3. Nc3 Nc6 4. d4 cxd4 5. Nxd4 g6 6. Nc2 d6 7. e4 {With "
	"this, the opening transposes into the Accelerated Dragon, except that "
	"White's pawn has gone to c4.} Bg7 8. Be2 Nd7 9. Bd2 O-O 10. O-O Nc5 11. "
	" b4 Bxc3 {The obvious drawback to capturing on c3 and then e4 is that it "
	"leaves the black squares on the kingside vulnerable to White's dark-squared "
	"bishop.} 12. Bxc3 Nxe4 13. Bb2 f5 (13... Be6 {is the most frequently tried, "
	"though not with any great success.}) 14. Kh1 a5 ({Black might try to impede "
	"the range of the b2 bishop with} 14... e5 {but} 15. f3 Nf6 16. b5 Na5 "
	"17. Ba3 {transfers the pressure to the a3-f8 diogonal and the weak pawn "
	"on d6, so White has compenasation for the pawn.}) 15. b5 Ne5 16. Qd4 Nf6 "
	"17. f4 Nf7 18. Rad1 Qc7 19. Qf2 e5 20. Qh4 Ne4 21. Ne3 {Diagram [#]} g5 "
	"$6 {It is perhaps surprising that Black didn't think to preface this advance "
	"with} (21... Be6 {when White can continue with} 22. Bd3 {and the position "
	"looks to offer chances to both players.}) 22. fxg5 Nfxg5 23. Nd5 Qd8 (23..."
	" Qg7 {looks more solid.}) 24. Bd3 Re8 (24... Nc5 {again runs into} 25."
	"Nb6 $1 {and all the tactics work out in White's favour.}) 25. c5 $1 Nxc5 "
	"26. Bxf5 Bxf5 27. Rxf5 Nge4 28. Qg4+ Kh8 29. Rdf1 Re6 30. Qh3 Qg8 {Diagram "
	"[#] In desperation, Black decides that an exchange sacrifice might be "
	"worth trying, to eliminate White's strong knight.} 31. Nc7 Rae8 32. Nxe8 "
	"Rxe8 33. Qh4 Qg6 34. Rf8+ Rxf8 35. Rxf8+ Kg7 36. Qe7+ Kh6 37. Qh4+ Kg7 "
	"38. Qe7+ Kh6 39. Bc1+ Kh5 40. Rf3 Ng5 41. g4+ {1-0} {If} Kxg4 42. Rg3+ "
	"Kh4 43. Qxg5+ 1-0";
	
	auto i = myString.cbegin();
	Token t = nextToken(i, myString.cend());
	
	while (t.type != TokenInvalid) {
		std::cout << "Token: " << std::hex << t.type << " <" << t.contents << "> ";
		if (t.type == TokenGenericMove) {
			std::cout << std::hex << t.subType;
		}
		std::cout << std::endl;
		i += t.charactersConsumed;
		
		try {
			t = nextToken(i, myString.end());
		} catch (std::exception & e) {
			std::cout << e.what() << std::endl;
		}
	}
	
}