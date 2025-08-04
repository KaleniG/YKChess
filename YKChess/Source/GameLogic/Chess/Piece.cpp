#include "GameLogic/Chess/Piece.h"

namespace yk
{
  namespace Chess
  {

    Piece::Piece(Type type, Side side)
      : m_Type(type), m_Side(side) {}

  }
}