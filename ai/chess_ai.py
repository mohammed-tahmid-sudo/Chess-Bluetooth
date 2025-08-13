# Chess AI Module
try:
    import chess
    import chess.engine
    CHESS_MODULE_AVAILABLE = True
except ImportError:
    CHESS_MODULE_AVAILABLE = False
    print("Warning: python-chess module not available. AI functionality will be limited.")

class ChessAI:
    def __init__(self):
        if CHESS_MODULE_AVAILABLE:
            self.board = chess.Board()
        else:
            self.board = None

    def get_best_move(self):
        """Get the best move using basic AI logic"""
        if not CHESS_MODULE_AVAILABLE or self.board is None:
            return None
            
        # Simple AI: return the first legal move
        legal_moves = list(self.board.legal_moves)
        if legal_moves:
            return legal_moves[0]
        return None
    
    def make_move(self, move):
        """Make a move on the board"""
        if CHESS_MODULE_AVAILABLE and self.board is not None:
            try:
                self.board.push_san(move) if isinstance(move, str) else self.board.push(move)
                return True
            except:
                return False
        return False
    
    def is_game_over(self):
        """Check if the game is over"""
        if CHESS_MODULE_AVAILABLE and self.board is not None:
            return self.board.is_game_over()
        return False
    
    def get_board_state(self):
        """Get current board state as string"""
        if CHESS_MODULE_AVAILABLE and self.board is not None:
            return str(self.board)
        return "Board not available"
