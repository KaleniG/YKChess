#include <bit>

#include <YKLib.h>
#include <glm/glm.hpp>

#include "Core/WindowManager.h"
#include "GameLogic/Chess/Game.h"
#include "Rendering/Renderer.h"

// Macro removes the 0b prefix that cannot be 'd, easier to read this way
#define b(number) 0b##number

// Macros for pieces default positions
#define BLACK_PAWNS_DEFAULT_POSITIONS   b(00000000'11111111'00000000'00000000'00000000'00000000'00000000'00000000)
#define BLACK_ROOKS_DEFAULT_POSITIONS   b(10000001'00000000'00000000'00000000'00000000'00000000'00000000'00000000)
#define BLACK_KNIGHTS_DEFAULT_POSITIONS b(01000010'00000000'00000000'00000000'00000000'00000000'00000000'00000000)
#define BLACK_BISHOPS_DEFAULT_POSITIONS b(00100100'00000000'00000000'00000000'00000000'00000000'00000000'00000000)
#define BLACK_QUEEN_DEFAULT_POSITION    b(00010000'00000000'00000000'00000000'00000000'00000000'00000000'00000000)
#define BLACK_KING_DEFAULT_POSITION     b(00001000'00000000'00000000'00000000'00000000'00000000'00000000'00000000)

#define WHITE_PAWNS_DEFAULT_POSITIONS   b(00000000'00000000'00000000'00000000'00000000'00000000'11111111'00000000)
#define WHITE_ROOKS_DEFAULT_POSITIONS   b(00000000'00000000'00000000'00000000'00000000'00000000'00000000'10000001)
#define WHITE_KNIGHTS_DEFAULT_POSITIONS b(00000000'00000000'00000000'00000000'00000000'00000000'00000000'01000010)
#define WHITE_BISHOPS_DEFAULT_POSITIONS b(00000000'00000000'00000000'00000000'00000000'00000000'00000000'00100100)
#define WHITE_QUEEN_DEFAULT_POSITION    b(00000000'00000000'00000000'00000000'00000000'00000000'00000000'00010000)
#define WHITE_KING_DEFAULT_POSITION     b(00000000'00000000'00000000'00000000'00000000'00000000'00000000'00001000)

namespace yk
{
  namespace Chess
  {
    std::shared_ptr<Game> Game::Create()
    {
      std::shared_ptr<Game> game(new Game());
      game->SetPiecesDefaultPositions();
      game->m_ChessAtlas = ImageResource::Create("Assets/Textures/ChessAtlas.png", 24, 24);
      Renderer::SetImageSlot(1, game->m_ChessAtlas);
      game->DrawGame();
      Renderer::EndBatch();
      return game;
    }

    void Game::SetPiecesDefaultPositions()
    {
      m_BoardStatus.BlackPawns = BLACK_PAWNS_DEFAULT_POSITIONS;
      m_BoardStatus.BlackRooks = BLACK_ROOKS_DEFAULT_POSITIONS;
      m_BoardStatus.BlackKnights = BLACK_KNIGHTS_DEFAULT_POSITIONS;
      m_BoardStatus.BlackBishops = BLACK_BISHOPS_DEFAULT_POSITIONS;
      m_BoardStatus.BlackQueens = BLACK_QUEEN_DEFAULT_POSITION;
      m_BoardStatus.BlackKing = BLACK_KING_DEFAULT_POSITION;

      m_BoardStatus.WhitePawns = WHITE_PAWNS_DEFAULT_POSITIONS;
      m_BoardStatus.WhiteRooks = WHITE_ROOKS_DEFAULT_POSITIONS;
      m_BoardStatus.WhiteKnights = WHITE_KNIGHTS_DEFAULT_POSITIONS;
      m_BoardStatus.WhiteBishops = WHITE_BISHOPS_DEFAULT_POSITIONS;
      m_BoardStatus.WhiteQueens = WHITE_QUEEN_DEFAULT_POSITION;
      m_BoardStatus.WhiteKing = WHITE_KING_DEFAULT_POSITION;
    }

    Game::BoardBitField Game::GetFullBoard() const
    {
      return Game::GetBlackPieces() | Game::GetWhitePieces();
    }

    Game::BoardBitField Game::GetBlackPieces() const
    {
      return m_BoardStatus.BlackPawns | m_BoardStatus.BlackRooks | m_BoardStatus.BlackKnights | m_BoardStatus.BlackBishops | m_BoardStatus.BlackQueens | m_BoardStatus.BlackKing;
    }

    Game::BoardBitField Game::GetWhitePieces() const
    {
      return m_BoardStatus.WhitePawns | m_BoardStatus.WhiteRooks | m_BoardStatus.WhiteKnights | m_BoardStatus.WhiteBishops | m_BoardStatus.WhiteQueens | m_BoardStatus.WhiteKing;
    }

    Game::BoardBitField Game::GetPosition(int32_t row, int32_t col) const
    {
      YK_ASSERT(row < 8 && row >= 0 && col < 8 && col >= 0, "Trying to access a value outside of limits: row={} col={}", row, col);
      return 1ULL << (((7 - row) * 8) + (7 - col));
    }

    std::tuple<int32_t, int32_t> Game::GetPosition(BoardBitField tile) const
    {
      YK_ASSERT(tile != 0 && (tile & (tile - 1)) == 0, "Tile must have exactly one bit set");

      int32_t index = static_cast<int32_t>(std::countr_zero(tile));

      int32_t row = static_cast<int32_t>(7 - (index / 8));
      int32_t col = static_cast<int32_t>(7 - (index % 8));

      return { row, col };
    }

    std::tuple<Game::Piece, Game::Side> Game::AccessTile(BoardBitField tile) const
    {
      YK_ASSERT(tile != 0 && (tile & (tile - 1)) == 0, "Tile must have exactly one bit set");

      if (m_BoardStatus.BlackPawns & tile)   return { Piece::Pawn,   Side::Black };
      if (m_BoardStatus.BlackRooks & tile)   return { Piece::Rook,   Side::Black };
      if (m_BoardStatus.BlackKnights & tile) return { Piece::Knight, Side::Black };
      if (m_BoardStatus.BlackBishops & tile) return { Piece::Bishop, Side::Black };
      if (m_BoardStatus.BlackQueens & tile)  return { Piece::Queen,  Side::Black };
      if (m_BoardStatus.BlackKing & tile)    return { Piece::King,   Side::Black };

      if (m_BoardStatus.WhitePawns & tile)   return { Piece::Pawn,   Side::White };
      if (m_BoardStatus.WhiteRooks & tile)   return { Piece::Rook,   Side::White };
      if (m_BoardStatus.WhiteKnights & tile) return { Piece::Knight, Side::White };
      if (m_BoardStatus.WhiteBishops & tile) return { Piece::Bishop, Side::White };
      if (m_BoardStatus.WhiteQueens & tile)  return { Piece::Queen,  Side::White };
      if (m_BoardStatus.WhiteKing & tile)    return { Piece::King,   Side::White };

      return { Piece::None, Side::None };
    }

    std::tuple<Game::Piece, Game::Side> Game::AccessTile(int32_t row, int32_t col) const
    {
      return Game::AccessTile(Game::GetPosition(row, col));
    }

    Game::BoardBitField Game::GetPieceMoves(BoardBitField tile, bool attacks) const
    {
      BoardBitField moves = 0ULL;

      const auto [piece, side] = Game::AccessTile(tile);
      const auto [row, col] = Game::GetPosition(tile);

      switch (piece)
      {
      case Piece::None:
      {
        break;
      }
      case Piece::Pawn:
      {
        switch (side)
        {
        case Side::Black:
        {
          if (row + 1 < 8)
            if (!(Game::GetFullBoard() & Game::GetPosition(row + 1, col)))
              moves |= Game::GetPosition(row + 1, col);

          if (attacks)
          {
            if (row + 1 < 8 && col - 1 >= 0)
              if (Game::GetWhitePieces() & Game::GetPosition(row + 1, col - 1))
                moves |= Game::GetPosition(row + 1, col - 1);

            if (row + 1 < 8 && col + 1 < 8)
              if (Game::GetWhitePieces() & Game::GetPosition(row + 1, col + 1))
                moves |= Game::GetPosition(row + 1, col + 1);
          }

          if (tile & BLACK_PAWNS_DEFAULT_POSITIONS && row + 2 < 8)
            if (!(Game::GetFullBoard() & (Game::GetPosition(row + 1, col) | Game::GetPosition(row + 2, col))))
              moves |= Game::GetPosition(row + 2, col);

          break;
        }
        case Side::White:
        {
          if (row - 1 >= 0)
            if (!(Game::GetFullBoard() & Game::GetPosition(row - 1, col)))
              moves |= Game::GetPosition(row - 1, col);

          if (attacks)
          {
            if (row - 1 >= 0 && col - 1 >= 0)
              if (Game::GetBlackPieces() & Game::GetPosition(row - 1, col - 1))
                moves |= Game::GetPosition(row - 1, col - 1);

            if (row - 1 >= 0 && col + 1 < 8)
              if (Game::GetBlackPieces() & Game::GetPosition(row - 1, col + 1))
                moves |= Game::GetPosition(row - 1, col + 1);
          }

          if (tile & WHITE_PAWNS_DEFAULT_POSITIONS && row - 2 >= 0)
            if (!(Game::GetFullBoard() & (Game::GetPosition(row - 1, col) | Game::GetPosition(row - 2, col))))
              moves |= Game::GetPosition(row - 2, col);

          break;
        }
        }
        break;
      }
      case Piece::Queen:
      case Piece::Rook:
      {
        BoardBitField movesField = 0ULL;

        // HORIZONTAL CHECK
        BoardRowBitField fullRow = Game::GetBoardRow(Game::GetFullBoard(), row);
        BoardRowBitField enemyRow = Game::GetBoardRow((side == Side::Black) ? Game::GetWhitePieces() : Game::GetBlackPieces(), row);
        BoardRowBitField rookRow = 0;

        if ((rookRow = Game::GetBoardRow(tile, row) << 1) != 0)
        {
          while (!(rookRow & fullRow) && rookRow != 0)
          {
            movesField |= Game::GetRowBoard(rookRow, row);
            rookRow <<= 1;
          }

          if (rookRow != 0 && attacks && rookRow & enemyRow)
            movesField |= Game::GetRowBoard(rookRow, row);
        }
          
        if ((rookRow = Game::GetBoardRow(tile, row) >> 1) != 0)
        {
          while (!(rookRow & fullRow) && rookRow != 0)
          {
            movesField |= Game::GetRowBoard(rookRow, row);
            rookRow >>= 1;
          }

          if (rookRow != 0 && attacks && rookRow & enemyRow)
            movesField |= Game::GetRowBoard(rookRow, row);
        }

        // VERTICAL CHECK
        BoardColumnBitField fullColumn = Game::GetBoardColumn(Game::GetFullBoard(), col);
        BoardColumnBitField enemyColumn = Game::GetBoardColumn((side == Side::Black) ? Game::GetWhitePieces() : Game::GetBlackPieces(), col);
        BoardColumnBitField rookColumn;

        if ((rookColumn = Game::GetBoardColumn(tile, col) << 1) != 0)
        {
          while (!(rookColumn & fullColumn) && rookColumn != 0)
          {
            movesField |= Game::GetColumnBoard(rookColumn, col);
            rookColumn <<= 1;
          }

          if (rookColumn != 0 && attacks && rookColumn & enemyColumn)
            movesField |= Game::GetColumnBoard(rookColumn, col);
        }

        if ((rookColumn = Game::GetBoardColumn(tile, col) >> 1) != 0)
        {
          while (!(rookColumn & fullColumn) && rookColumn != 0)
          {
            movesField |= Game::GetColumnBoard(rookColumn, col);
            rookColumn >>= 1;
          }

          if (rookColumn != 0 && attacks && rookColumn & enemyColumn)
            movesField |= Game::GetColumnBoard(rookColumn, col);
        }

        moves |= movesField;

        if (piece != Piece::Queen)
          break;
      }
      case Piece::Bishop:
      {
        BoardBitField movesField = 0ULL;

        auto [diag1Full, diag2Full] = Game::GetBoardDiagonals(Game::GetFullBoard(), row, col);
        auto [diag1Enemy, diag2Enemy] = Game::GetBoardDiagonals((side == Side::Black) ? Game::GetWhitePieces() : Game::GetBlackPieces(), row, col);
        auto [diag1Bishop, diag2Bishop] = Game::GetBoardDiagonals(tile, row, col);

        // FIRST DIAGONAL
        BoardDiagonalBitField d1;

        if ((d1 = diag1Bishop << 1) != 0)
        {
          while (!(d1 & diag1Full) && d1 != 0)
          {
            movesField |= Game::GetDiagonalsBoard(d1, 0, row, col);
            d1 <<= 1;
          }

          if (d1 != 0 && attacks && (d1 & diag1Enemy))
            movesField |= Game::GetDiagonalsBoard(d1, 0, row, col);
        }
          
        if ((d1 = diag1Bishop >> 1) != 0)
        {
          while (!(d1 & diag1Full) && d1 != 0)
          {
            movesField |= Game::GetDiagonalsBoard(d1, 0, row, col);
            d1 >>= 1;
          }

          if (d1 != 0 && attacks && (d1 & diag1Enemy))
            movesField |= Game::GetDiagonalsBoard(d1, 0, row, col);
        }

        // SECOND DIAGONAL
        BoardDiagonalBitField d2;

        if ((d2 = diag2Bishop << 1) != 0)
        {
          while (!(d2 & diag2Full) && d2 != 0)
          {
            movesField |= Game::GetDiagonalsBoard(0, d2, row, col);
            d2 <<= 1;
          }

          if (d2 != 0 && attacks && (d2 & diag2Enemy))
            movesField |= Game::GetDiagonalsBoard(0, d2, row, col);
        }
          

        if ((d2 = diag2Bishop >> 1) != 0)
        {
          while (!(d2 & diag2Full) && d2 != 0)
          {
            movesField |= Game::GetDiagonalsBoard(0, d2, row, col);
            d2 >>= 1;
          }

          if (d2 != 0 && attacks && (d2 & diag2Enemy))
            movesField |= Game::GetDiagonalsBoard(0, d2, row, col);
        }

        moves |= movesField;
        break;
      }
      case Piece::Knight:
      {
        const std::array<std::tuple<int32_t, int32_t>, 8> displaces = { {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}} };

        for (const auto& [dr, dc] : displaces)
        {
          int32_t r = row + dr, c = col + dc;
          if (r < 0 || r >= 8 || c < 0 || c >= 8) continue;

          BoardBitField pos = GetPosition(r, c);
          if (!(pos & GetFullBoard()) || (attacks && (pos & ((side == Side::Black) ? GetWhitePieces() : GetBlackPieces()))))
            moves |= pos;
        }
        break;
      }
      case Piece::King:
      {
        const std::array<std::tuple<int32_t, int32_t>, 8> displaces = { {{1, 1}, {1, 0}, {1, -1}, {0, 1}, {0, -1}, {-1, 1}, {-1, 0}, {-1, -1}} };

        for (int32_t i = 0; i < displaces.size(); i++)
        {
          const auto [rowDisp, colDisp] = displaces[i];
          if (row + rowDisp >= 0 && row + rowDisp < 8 && col + colDisp >= 0 && col + colDisp < 8)
          {
            BoardBitField pos = Game::GetPosition(row + rowDisp, col + colDisp);
            if (!(pos & Game::GetFullBoard()) || (attacks && pos & ((side == Side::Black) ? Game::GetWhitePieces() : Game::GetBlackPieces())))
              moves |= pos;
          }
        }
        break;
      }
      }

      return moves;
    }

    Game::BoardBitField Game::GetPieceMoves(int32_t row, int32_t col, bool attacks) const
    {
      return Game::GetPieceMoves(Game::GetPosition(row, col), attacks);
    }

    Game::BoardRowBitField Game::GetBoardRow(BoardBitField board, int32_t row) const
    {
      return static_cast<BoardRowBitField>((board >> ((7 - row) * 8)) & 0xFF);
    }

    Game::BoardColumnBitField Game::GetBoardColumn(BoardBitField board, int32_t col) const
    {
      BoardColumnBitField field = 0;

      for (int32_t row = 0; row < 8; row++)
      {
        int32_t shift = ((7 - row) * 8) + (7 - col);
        uint8_t bit = static_cast<uint8_t>((board >> shift) & 1ULL);
        field |= (bit << (7 - row));
      }

      return field;
    }

    std::tuple<Game::BoardDiagonalBitField, Game::BoardDiagonalBitField> Game::GetBoardDiagonals(BoardBitField board, int32_t row, int32_t col) const
    {
      YK_ASSERT(row < 8 && row >= 0 && col < 8 && col >= 0, "Trying to access a value outside of limits: row={} col={}", row, col);

      BoardDiagonalBitField d1 = 0;
      BoardDiagonalBitField d2 = 0;

      int32_t delta1 = col - row;
      for (int32_t r = 0; r < 8; r++)
      {
        int32_t c = r + delta1;
        if (c >= 0 && c < 8)
        {
          int32_t shift = ((7 - r) * 8) + (7 - c);
          uint8_t bit = static_cast<uint8_t>((board >> shift) & 1ULL);
          d1 |= (bit << (7 - r));
        }
      }

      int32_t delta2 = row + col;
      for (int32_t r = 0; r < 8; r++)
      {
        int32_t c = delta2 - r;
        if (c >= 0 && c < 8)
        {
          int32_t shift = ((7 - r) * 8) + (7 - c);
          uint8_t bit = static_cast<uint8_t>((board >> shift) & 1ULL);
          d2 |= (bit << (7 - r));
        }
      }

      return { d1, d2 };
    }

    Game::BoardBitField Game::GetRowBoard(BoardRowBitField row_field, int32_t row) const
    {
      return (static_cast<BoardBitField>(row_field) & 0xFFULL) << ((7 - row) * 8);
    }

    Game::BoardBitField Game::GetColumnBoard(BoardColumnBitField col_field, int32_t col) const
    {
      BoardBitField board = 0ULL;

      for (int32_t row = 0; row < 8; row++)
      {
        uint8_t bit = static_cast<uint8_t>((col_field >> (7 - row)) & 1ULL);
        int32_t shift = ((7 - row) * 8) + (7 - col);
        board |= (static_cast<BoardBitField>(bit) << shift);
      }

      return board;
    }

    Game::BoardBitField Game::GetDiagonalsBoard(BoardDiagonalBitField first, BoardDiagonalBitField second, int32_t row, int32_t col) const
    {
      YK_ASSERT(row < 8 && row >= 0 && col < 8 && col >= 0, "Trying to access a value outside of limits: row={} col={}", row, col);

      BoardBitField board = 0ULL;

      if (first)
      {
        int32_t delta1 = col - row;
        for (int32_t r = 0; r < 8; r++)
        {
          int32_t c = r + delta1;
          if (c >= 0 && c < 8)
          {
            uint8_t bit = static_cast<uint8_t>((first >> (7 - r)) & 1ULL);
            if (!bit) continue;
            int32_t shift = ((7 - r) * 8) + (7 - c);
            board |= (static_cast<BoardBitField>(bit) << shift);
          }
        }
      }

      if (second)
      {
        int32_t delta2 = row + col;
        for (int32_t r = 0; r < 8; r++)
        {
          int32_t c = delta2 - r;
          if (c >= 0 && c < 8)
          {
            uint8_t bit = static_cast<uint8_t>((second >> (7 - r)) & 1ULL);
            if (!bit) continue;
            int32_t shift = ((7 - r) * 8) + (7 - c);
            board |= (static_cast<BoardBitField>(bit) << shift);
          }
        }
      }

      return board;
    }

    void Game::MovePiece(BoardStatus& board, BoardBitField src_tile, BoardBitField dst_tile)
    {
      YK_ASSERT((src_tile != 0 && (src_tile & (src_tile - 1)) == 0) && (dst_tile != 0 && (dst_tile & (dst_tile - 1)) == 0), "Tiles must have exactly one bit set");

      m_BoardStatusHistory.push_back(m_BoardStatus);

      BoardBitField* boardToChange = nullptr;

      auto [piece, side] = Game::AccessTile(src_tile);
      switch (piece)
      {
      case Game::Piece::Pawn:
        boardToChange = (side == Side::Black) ? &board.BlackPawns : &board.WhitePawns;
        break;
      case Game::Piece::Rook:
        boardToChange = (side == Side::Black) ? &board.BlackRooks : &board.WhiteRooks;
        break;
      case Game::Piece::Knight:
        boardToChange = (side == Side::Black) ? &board.BlackKnights : &board.WhiteKnights;
        break;
      case Game::Piece::Bishop:
        boardToChange = (side == Side::Black) ? &board.BlackBishops : &board.WhiteBishops;
        break;
      case Game::Piece::Queen:
        boardToChange = (side == Side::Black) ? &board.BlackQueens : &board.WhiteQueens;
        break;
      case Game::Piece::King:
        boardToChange = (side == Side::Black) ? &board.BlackKing : &board.WhiteKing;
        break;
      default:
        YK_ASSERT(false, "Should not happend");
        return;
      }

      *boardToChange &= ~src_tile;
      *boardToChange |= dst_tile;

      // Clearing the destination tile from all other boards of the opposite side
      if (side == Side::Black)
      {
        board.WhitePawns &= ~dst_tile;
        board.WhiteRooks &= ~dst_tile;
        board.WhiteKnights &= ~dst_tile;
        board.WhiteBishops &= ~dst_tile;
        board.WhiteQueens &= ~dst_tile;
      }
      else
      {
        board.BlackPawns &= ~dst_tile;
        board.BlackRooks &= ~dst_tile;
        board.BlackKnights &= ~dst_tile;
        board.BlackBishops &= ~dst_tile;
        board.BlackQueens &= ~dst_tile;
      }
    }

    void Game::UpdateGameStatus(Side side)
    {
      auto for_each_bit = [&](BoardBitField bb, auto&& fn)
        {
        while (bb) 
        {
          BoardBitField lsb = bb & (~bb + 1);
          fn(lsb);
          bb &= (bb - 1);
        }
        };

      auto attacks_of = [&](Side attacker) -> BoardBitField 
        {
        BoardBitField attacks = 0ULL;

        BoardBitField pawns = (attacker == Side::White) ? m_BoardStatus.WhitePawns : m_BoardStatus.BlackPawns;
        BoardBitField rooks = (attacker == Side::White) ? m_BoardStatus.WhiteRooks : m_BoardStatus.BlackRooks;
        BoardBitField knights = (attacker == Side::White) ? m_BoardStatus.WhiteKnights : m_BoardStatus.BlackKnights;
        BoardBitField bishops = (attacker == Side::White) ? m_BoardStatus.WhiteBishops : m_BoardStatus.BlackBishops;
        BoardBitField queens = (attacker == Side::White) ? m_BoardStatus.WhiteQueens : m_BoardStatus.BlackQueens;
        BoardBitField king = (attacker == Side::White) ? m_BoardStatus.WhiteKing : m_BoardStatus.BlackKing;

        for_each_bit(pawns, [&](BoardBitField p) 
          {
          auto [r, c] = GetPosition(p);
          if (attacker == Side::White) 
          {
            if (r - 1 >= 0 && c - 1 >= 0) attacks |= GetPosition(r - 1, c - 1);
            if (r - 1 >= 0 && c + 1 < 8) attacks |= GetPosition(r - 1, c + 1);
          }
          else 
          {
            if (r + 1 < 8 && c - 1 >= 0) attacks |= GetPosition(r + 1, c - 1);
            if (r + 1 < 8 && c + 1 < 8) attacks |= GetPosition(r + 1, c + 1);
          }
          });

        auto add_moves = [&](BoardBitField pieces)
          {
          for_each_bit(pieces, [&](BoardBitField t)
            {
            attacks |= GetPieceMoves(t, true);
            });
          };

        add_moves(knights);
        add_moves(bishops);
        add_moves(rooks);
        add_moves(queens);
        add_moves(king);

        return attacks;
        };

      BoardBitField whiteAttacks = attacks_of(Side::White);
      BoardBitField blackAttacks = attacks_of(Side::Black);

      m_GameStatus.WhiteCheck = (blackAttacks & m_BoardStatus.WhiteKing) != 0ULL;
      m_GameStatus.BlackCheck = (whiteAttacks & m_BoardStatus.BlackKing) != 0ULL;

      m_GameStatus.Mate = false;

      bool sideInCheck = (side == Side::White) ? m_GameStatus.WhiteCheck : m_GameStatus.BlackCheck;
      if (!sideInCheck) 
        return;

      BoardBitField sidePieces = (side == Side::White) ? Game::GetWhitePieces() : Game::GetBlackPieces();

      bool hasEscape = false;

      for_each_bit(sidePieces, [&](BoardBitField src) 
        {
        if (hasEscape) 
          return;

        BoardBitField moves = GetPieceMoves(src, true);

        for_each_bit(moves, [&](BoardBitField dst) 
          {
          if (hasEscape) 
            return;

          BoardStatus backup = m_BoardStatus;
          Game::MovePiece(m_BoardStatus, src, dst);

          BoardBitField oppAttacks = attacks_of((side == Side::White) ? Side::Black : Side::White);
          bool stillInCheck = (oppAttacks & ((side == Side::White) ? m_BoardStatus.WhiteKing : m_BoardStatus.BlackKing)) != 0ULL;

          if (!stillInCheck) 
            hasEscape = true;

          m_BoardStatus = backup;
          });
        });

      m_GameStatus.Mate = !hasEscape;
    }

    void Game::Draw(Piece piece, Side side, int32_t row, int32_t col) const
    {
      const glm::vec3 pos(-0.7f + (0.2f * col), -0.7f + (0.2f * row), 0.0f);
      const glm::vec3 scale(0.2f, 0.2f, 1.0f);
      const int32_t id = row * 8 + col + 1;
      SubTexture subTexture;

      switch (piece)
      {
      case Game::Piece::Pawn:
        subTexture = m_ChessAtlas->GetSubTexture((side == Side::Black) ? 9 : 3);
        break;
      case Game::Piece::Rook:
        subTexture = m_ChessAtlas->GetSubTexture((side == Side::Black) ? 10 : 4);
        break;
      case Game::Piece::Knight:
        subTexture = m_ChessAtlas->GetSubTexture((side == Side::Black) ? 8 : 2);
        break;
      case Game::Piece::Bishop:
        subTexture = m_ChessAtlas->GetSubTexture((side == Side::Black) ? 11 : 5);
        break;
      case Game::Piece::Queen:
        subTexture = m_ChessAtlas->GetSubTexture((side == Side::Black) ? 13 : 7);
        break;
      case Game::Piece::King:
        subTexture = m_ChessAtlas->GetSubTexture((side == Side::Black) ? 12 : 6);
        break;
      default:
        subTexture = m_ChessAtlas->GetSubTexture(37);
        break;
      }

      Renderer::DrawImage(pos, scale, id, subTexture);
    }

    void Game::Draw(DrawElement element, float x, float y, int32_t id) const
    {
      const glm::vec3 pos(x, y, -0.5f);
      const glm::vec3 scale(0.2f, 0.2f, 1.0f);
      SubTexture subTexture;

      switch (element)
      {
      case Game::DrawElement::BlackTile:
        subTexture = m_ChessAtlas->GetSubTexture(0);
        break;
      case Game::DrawElement::WhiteTile:
        subTexture = m_ChessAtlas->GetSubTexture(1);
        break;
      case Game::DrawElement::Num1:
        subTexture = m_ChessAtlas->GetSubTexture(25);
        break;
      case Game::DrawElement::Num2:
        subTexture = m_ChessAtlas->GetSubTexture(26);
        break;
      case Game::DrawElement::Num3:
        subTexture = m_ChessAtlas->GetSubTexture(27);
        break;
      case Game::DrawElement::Num4:
        subTexture = m_ChessAtlas->GetSubTexture(28);
        break;
      case Game::DrawElement::Num5:
        subTexture = m_ChessAtlas->GetSubTexture(29);
        break;
      case Game::DrawElement::Num6:
        subTexture = m_ChessAtlas->GetSubTexture(30);
        break;
      case Game::DrawElement::Num7:
        subTexture = m_ChessAtlas->GetSubTexture(31);
        break;
      case Game::DrawElement::Num8:
        subTexture = m_ChessAtlas->GetSubTexture(32);
        break;
      case Game::DrawElement::CharA:
        subTexture = m_ChessAtlas->GetSubTexture(16);
        break;
      case Game::DrawElement::CharB:
        subTexture = m_ChessAtlas->GetSubTexture(17);
        break;
      case Game::DrawElement::CharC:
        subTexture = m_ChessAtlas->GetSubTexture(18);
        break;
      case Game::DrawElement::CharD:
        subTexture = m_ChessAtlas->GetSubTexture(19);
        break;
      case Game::DrawElement::CharE:
        subTexture = m_ChessAtlas->GetSubTexture(20);
        break;
      case Game::DrawElement::CharF:
        subTexture = m_ChessAtlas->GetSubTexture(21);
        break;
      case Game::DrawElement::CharG:
        subTexture = m_ChessAtlas->GetSubTexture(22);
        break;
      case Game::DrawElement::CharH:
        subTexture = m_ChessAtlas->GetSubTexture(23);
        break;
      case Game::DrawElement::Corner:
        subTexture = m_ChessAtlas->GetSubTexture(24);
        break;
      case Game::DrawElement::HoverTile:
        subTexture = m_ChessAtlas->GetSubTexture(15);
        break;
      case Game::DrawElement::ActionTile:
        subTexture = m_ChessAtlas->GetSubTexture(14);
        break;
      default:
      {
        YK_WARN("Drawing invisible tile");
        subTexture = m_ChessAtlas->GetSubTexture(37);
        break;
      }
      }

      Renderer::DrawImage(pos, scale, id, subTexture);
    }

    void Game::DrawGame() const
    {
      Game::Draw(DrawElement::Corner, -0.793f, -0.9f);
      Game::Draw(DrawElement::CharA, -0.7f, -0.9f);
      Game::Draw(DrawElement::CharB, -0.5f, -0.9f);
      Game::Draw(DrawElement::CharC, -0.3f, -0.9f);
      Game::Draw(DrawElement::CharD, -0.1f, -0.9f);
      Game::Draw(DrawElement::CharE, 0.1f, -0.9f);
      Game::Draw(DrawElement::CharF, 0.3f, -0.9f);
      Game::Draw(DrawElement::CharG, 0.5f, -0.9f);
      Game::Draw(DrawElement::CharH, 0.7f, -0.9f);
      Game::Draw(DrawElement::Corner, 0.9f, -0.9f);

      Game::Draw(DrawElement::Corner, -0.793f, 0.793);
      Game::Draw(DrawElement::CharA, -0.7f, 0.793);
      Game::Draw(DrawElement::CharB, -0.5f, 0.793);
      Game::Draw(DrawElement::CharC, -0.3f, 0.793);
      Game::Draw(DrawElement::CharD, -0.1f, 0.793);
      Game::Draw(DrawElement::CharE, 0.1f, 0.793);
      Game::Draw(DrawElement::CharF, 0.3f, 0.793);
      Game::Draw(DrawElement::CharG, 0.5f, 0.793);
      Game::Draw(DrawElement::CharH, 0.7f, 0.793);
      Game::Draw(DrawElement::Corner, 0.9f, 0.793);

      for (int32_t row = 0; row < 8; row++)
      {
        Game::Draw(static_cast<DrawElement>(row + 2), -0.793f, -0.7f + (0.2f * row));
        for (int32_t col = 0; col < 8; col++)
        {
          Game::Draw(((row + col) % 2) ? DrawElement::BlackTile : DrawElement::WhiteTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row));
          const auto [piece, side] = Game::AccessTile(row, col);
          Game::Draw(piece, side, row, col);
        }
        Game::Draw(static_cast<DrawElement>(row + 2), 0.9f, -0.7f + (0.2f * row));
      }
    }

    void Game::OnMouseMove(double xpos, double ypos)
    {
      uint32_t id = Renderer::GetPositionID(EventManager::MouseNormalizedToPixel(xpos, ypos));

      if (id == 0)
      {
        if (m_HoveringTile != 0ULL)
        {
          Renderer::ResetBatch();
          Game::DrawGame();

          if (m_SelectedTile)
          {
            auto [row, col] = Game::GetPosition(m_SelectedTile);
            Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

            if (m_SelectedTileMoves)
              for (int32_t row = 0; row < 8; row++)
                for (int32_t col = 0; col < 8; col++)
                  if (Game::GetPosition(row, col) & m_SelectedTileMoves)
                    Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);
          }
          if (m_NextMoveTile)
          {
            auto [row, col] = Game::GetPosition(m_NextMoveTile);
            Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);
          }
          if (m_GameStatus.BlackCheck)
          {
            auto [rowk, colk] = Game::GetPosition(m_BoardStatus.BlackKing);
            Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
          }
          if (m_GameStatus.WhiteCheck)
          {
            auto [rowk, colk] = Game::GetPosition(m_BoardStatus.WhiteKing);
            Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
          }

          Renderer::EndBatch();
          m_HoveringTile = 0ULL;
        }
        return;
      }

      glm::uvec2 mouseTilePos = glm::uvec2(7 - ((id - 1) / 8), (id - 1) % 8);

      if (m_HoveringTile)
      {
        auto [row, col] = Game::GetPosition(m_HoveringTile);

        if (mouseTilePos.x == row && mouseTilePos.y == col)
          return;
      }

      m_HoveringTile = Game::GetPosition(mouseTilePos.x, mouseTilePos.y);

      Renderer::ResetBatch();
      Game::DrawGame();
      if (m_HoveringTile)
      {
        auto [row, col] = Game::GetPosition(m_HoveringTile);
        Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);
      }
      if (m_SelectedTile)
      {
        auto [row, col] = Game::GetPosition(m_SelectedTile);
        Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

        if (m_SelectedTileMoves)
          for (int32_t row = 0; row < 8; row++)
            for (int32_t col = 0; col < 8; col++)
              if (Game::GetPosition(row, col) & m_SelectedTileMoves)
                Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);
      }
      if (m_NextMoveTile)
      {
        auto [row, col] = Game::GetPosition(m_NextMoveTile);
        Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);
      }
      if (m_GameStatus.BlackCheck)
      {
        auto [rowk, colk] = Game::GetPosition(m_BoardStatus.BlackKing);
        Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
      }
      if (m_GameStatus.WhiteCheck)
      {
        auto [rowk, colk] = Game::GetPosition(m_BoardStatus.WhiteKing);
        Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
      }
      Renderer::EndBatch();
    }

    void Game::OnMouseButtonPress(MouseCode button)
    {
      switch (button)
      {
      case Mouse::ButtonLeft:
      {
        if (m_HoveringTile)
        {
          if (m_SelectedTile)
          {
            if (m_HoveringTile & m_SelectedTileMoves)
            {
              m_NextMoveTile = m_HoveringTile;

              Game::MovePiece(m_BoardStatus, m_SelectedTile, m_NextMoveTile);
              Game::UpdateGameStatus((m_Turn == Side::Black) ? Side::White : Side::Black);
              if ((m_Turn == Side::Black) ? m_GameStatus.BlackCheck : m_GameStatus.WhiteCheck)
              {
                m_BoardStatus = m_BoardStatusHistory.back();
                m_BoardStatusHistory.pop_back();
                m_NextMoveTile = 0ULL;
                break;
              }

              auto [row, col] = Game::GetPosition(m_NextMoveTile);
              Renderer::ResetBatch();
              Game::DrawGame();
              Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

              if (m_GameStatus.Mate)
              {
                if (m_GameStatus.BlackCheck)
                  YK_INFO("White side has won");
                if (m_GameStatus.WhiteCheck)
                  YK_INFO("Black side has won");
              }
              else
              {
                if (m_GameStatus.BlackCheck)
                {
                  auto [rowk, colk] = Game::GetPosition(m_BoardStatus.BlackKing);
                  Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
                }
                if (m_GameStatus.WhiteCheck)
                {
                  auto [rowk, colk] = Game::GetPosition(m_BoardStatus.WhiteKing);
                  Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
                }
              }

              Renderer::EndBatch();

              m_Turn = (m_Turn == Side::Black) ? Side::White : Side::Black;
              m_SelectedTile = 0ULL;
            }
            else
            {
              if (m_Turn == std::get<1>(Game::AccessTile(m_HoveringTile)))
              {
                m_NextMoveTile = 0ULL;
                m_SelectedTile = m_HoveringTile;
                auto [row, col] = Game::GetPosition(m_SelectedTile);

                Renderer::ResetBatch();
                Game::DrawGame();
                Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

                if (m_SelectedTileMoves = Game::GetPieceMoves(m_SelectedTile, true))
                  for (int32_t row = 0; row < 8; row++)
                    for (int32_t col = 0; col < 8; col++)
                      if (Game::GetPosition(row, col) & m_SelectedTileMoves)
                        Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

                if (m_GameStatus.BlackCheck)
                {
                  auto [rowk, colk] = Game::GetPosition(m_BoardStatus.BlackKing);
                  Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
                }
                if (m_GameStatus.WhiteCheck)
                {
                  auto [rowk, colk] = Game::GetPosition(m_BoardStatus.WhiteKing);
                  Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
                }

                Renderer::EndBatch();
              }
            }
          }
          else
          {
            if (m_Turn == std::get<1>(Game::AccessTile(m_HoveringTile)) && Game::GetPieceMoves(m_HoveringTile, true))
            {
              m_NextMoveTile = 0ULL;
              m_SelectedTile = m_HoveringTile;
              auto [row, col] = Game::GetPosition(m_SelectedTile);

              Renderer::ResetBatch();
              Game::DrawGame();
              Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

              if (m_SelectedTileMoves = Game::GetPieceMoves(m_SelectedTile, true))
                for (int32_t row = 0; row < 8; row++)
                  for (int32_t col = 0; col < 8; col++)
                    if (Game::GetPosition(row, col) & m_SelectedTileMoves)
                      Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

              if (m_GameStatus.BlackCheck)
              {
                auto [rowk, colk] = Game::GetPosition(m_BoardStatus.BlackKing);
                Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
              }
              if (m_GameStatus.WhiteCheck)
              {
                auto [rowk, colk] = Game::GetPosition(m_BoardStatus.WhiteKing);
                Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
              }

              Renderer::EndBatch();
            }
          }
        }
        break;
      }
      case Mouse::ButtonRight:
      {
        m_SelectedTile = 0ULL;
        m_NextMoveTile = 0ULL;

        Renderer::ResetBatch();
        Game::DrawGame();
        if (m_SelectedTile)
        {
          auto [row, col] = Game::GetPosition(m_SelectedTile);
          Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);

          if (m_SelectedTileMoves)
            for (int32_t row = 0; row < 8; row++)
              for (int32_t col = 0; col < 8; col++)
                if (Game::GetPosition(row, col) & m_SelectedTileMoves)
                  Game::Draw(DrawElement::HoverTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);
        }
        if (m_NextMoveTile)
        {
          auto [row, col] = Game::GetPosition(m_NextMoveTile);
          Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * col), -0.7f + (0.2f * row), row * 8 + col + 1);
        }
        if (m_GameStatus.BlackCheck)
        {
          auto [rowk, colk] = Game::GetPosition(m_BoardStatus.BlackKing);
          Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
        }
        if (m_GameStatus.WhiteCheck)
        {
          auto [rowk, colk] = Game::GetPosition(m_BoardStatus.WhiteKing);
          Game::Draw(DrawElement::ActionTile, -0.7f + (0.2f * colk), -0.7f + (0.2f * rowk), rowk * 8 + colk + 1);
        }
        Renderer::EndBatch();

        break;
      }
      }
    }
  }
}