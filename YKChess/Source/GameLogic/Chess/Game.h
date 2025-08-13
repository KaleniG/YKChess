#pragma once

#include <optional>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Core/EventManager.h"
#include "GameLogic/Chess/Piece.h"
#include "Rendering/ImageResource.h"

namespace yk
{
  namespace Chess
  {
    struct TileInfo
    {
      std::optional<Piece> TilePiece = std::nullopt;
      std::vector<glm::ivec2> PossiblePaths;
    };

    class Game : public EventManager
    {
    private:
      enum class Status
      {
        Hovering,
        Selecting
      };

    public:
      static std::unique_ptr<Game> Create();

    private:
      void HoverCurrentTile();
      bool SelectCurrentTile();
      void DrawBoard();
      void DrawPieces();

      TileInfo GetTileInfo(glm::ivec2 tile_pos, bool check_test) const;

      std::optional<glm::ivec2> FindKing(Side side) const;
      bool IsSquareAttacked(glm::ivec2 square, Side attacker) const;
      bool IsInCheck(Side side) const;
      bool IsCheckmate(Side side);

      void OnKeyPress(KeyCode key, bool repeat) final;

    private:
      std::shared_ptr<ImageResource> m_ChessAtlas;

      std::array<std::array<std::optional<Piece>, 8>, 8> m_Board;

      Side m_Turn = Side::White;
      glm::uvec2 m_CurrentCursorPos;
      TileInfo m_SelectedTile;
      int32_t m_SelectedTileIndex = 0;
      Status m_CurrentStatus = Status::Hovering;

      bool m_WhiteCheck = false;
      bool m_BlackCheck = false;
    };
  }
}