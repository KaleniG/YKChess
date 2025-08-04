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

      TileInfo GetTileInfo(glm::ivec2 tile_pos) const;

      void OnKeyPress(KeyCode key, bool repeat) final;

    private:
      std::shared_ptr<ImageResource> m_ChessAtlas;

      std::optional<Piece> m_Board[8][8];

      Side m_Turn = Side::White;
      glm::uvec2 m_CurrentCursorPos;
      TileInfo m_SelectedTile;
      int32_t m_SelectedTileIndex = 0;
      Status m_CurrentStatus = Status::Hovering;
    };
  }
}