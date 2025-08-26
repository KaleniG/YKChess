#pragma once

#include <optional>
#include <memory>
#include <tuple>

#include <glm/glm.hpp>

#include "Core/EventManager.h"
#include "Rendering/ImageResource.h"

namespace yk
{
  namespace Chess
  {
    class Game : public EventManager
    {
    private:
      using BoardBitField = uint64_t;
      using BoardRowBitField = uint8_t;
      using BoardColumnBitField = uint8_t;
      using BoardDiagonalBitField = uint8_t;

      struct GameStatus
      {
        bool Mate = false;
        bool WhiteCheck = false;
        bool BlackCheck = false;
      };
      
      struct BoardStatus
      {
        BoardBitField BlackPawns = 0ULL;
        BoardBitField BlackRooks = 0ULL;
        BoardBitField BlackKnights = 0ULL;
        BoardBitField BlackBishops = 0ULL;
        BoardBitField BlackQueens = 0ULL;
        BoardBitField BlackKing = 0ULL;

        BoardBitField WhitePawns = 0ULL;
        BoardBitField WhiteRooks = 0ULL;
        BoardBitField WhiteKnights = 0ULL;
        BoardBitField WhiteBishops = 0ULL;
        BoardBitField WhiteQueens = 0ULL;
        BoardBitField WhiteKing = 0ULL;
      };

      enum class Side : uint8_t
      {
        None,
        White,
        Black
      };

      enum class Piece : uint8_t
      {
        None,
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
        King
      };

      enum class DrawElement
      {
        BlackTile,
        WhiteTile,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        CharA,
        CharB,
        CharC,
        CharD,
        CharE,
        CharF,
        CharG,
        CharH,
        Corner,
        HoverTile,
        ActionTile
      };

    public:
      static std::shared_ptr<Game> Create();

    private:
      void SetPiecesDefaultPositions();

      BoardBitField GetFullBoard() const;
      BoardBitField GetBlackPieces() const;
      BoardBitField GetWhitePieces() const;

      BoardBitField GetPosition(int32_t row, int32_t col) const;
      std::tuple<int32_t, int32_t> GetPosition(BoardBitField tile) const;

      std::tuple<Piece, Side> AccessTile(BoardBitField tile) const;
      std::tuple<Piece, Side> AccessTile(int32_t row, int32_t col) const;

      BoardBitField GetPieceMoves(BoardBitField tile, bool attacks) const;
      BoardBitField GetPieceMoves(int32_t row, int32_t col, bool attacks) const;

      BoardRowBitField GetBoardRow(BoardBitField board, int32_t row) const;
      BoardColumnBitField GetBoardColumn(BoardBitField board, int32_t col) const;
      std::tuple<BoardDiagonalBitField, BoardDiagonalBitField> GetBoardDiagonals(BoardBitField board, int32_t row, int32_t col) const;

      BoardBitField GetRowBoard(BoardRowBitField row_field, int32_t row) const;
      BoardBitField GetColumnBoard(BoardColumnBitField col_field, int32_t col) const;
      BoardBitField GetDiagonalsBoard(BoardDiagonalBitField first, BoardDiagonalBitField second, int32_t row, int32_t col) const;

      void MovePiece(BoardStatus& board, BoardBitField src_tile, BoardBitField dst_tile);
      void UpdateGameStatus(Side side);

      void Draw(Piece piece, Side side, int32_t row, int32_t col) const;
      void Draw(DrawElement element, float x, float y, int32_t id = 0) const;

      void DrawGame() const;

      void OnMouseMove(double xpos, double ypos) final;
      void OnMouseButtonPress(MouseCode button) final;

    private:
      Game() = default;
      Game(const Game&) = delete;
      Game& operator=(const Game&) = delete;
      Game(Game&&) = delete;
      Game& operator=(Game&&) = delete;

    private:
      std::shared_ptr<ImageResource> m_ChessAtlas;

      BoardBitField m_HoveringTile = 0ULL;
      BoardBitField m_SelectedTile = 0ULL;
      BoardBitField m_SelectedTileMoves = 0ULL;
      BoardBitField m_NextMoveTile = 0ULL;

      BoardStatus m_BoardStatus;
      std::vector<BoardStatus> m_BoardStatusHistory;

      GameStatus m_GameStatus;
      Side m_Turn = Side::White;
    };
  }
}