#pragma once

namespace yk
{
  namespace Chess
  {
    enum class Side
    {
      White,
      Black
    };

    class Piece
    {
    public:
      enum class Type
      {
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
        King
      };

    public:
      Piece(Type type, Side side);

      Side GetSide() const { return m_Side; }
      Type GetType() const { return m_Type; }

    private:
      Type m_Type;
      Side m_Side;
    };
  }
}