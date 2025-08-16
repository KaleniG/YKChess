#include "Core/WindowManager.h"
#include "Core/Input.h"
#include "GameLogic/Chess/Game.h"
#include "Rendering/Renderer.h"

namespace yk
{
  namespace Chess
  {
    std::unique_ptr<Game> Game::Create()
    {
      std::unique_ptr<Game> game = std::make_unique<Game>();

      game->m_CurrentCursorPos = glm::ivec2(7, 7);

      game->m_ChessAtlas = ImageResource::Create("Assets/Textures/ChessAtlas.png", 24, 24);
      Renderer::SetImageSlot(1, game->m_ChessAtlas);

      for (int32_t y = 0; y < 8; y++)
        for (int32_t x = 0; x < 8; x++)
          game->m_Board[x][y] = std::nullopt;

      game->m_Board[0][0].emplace(Chess::Piece::Type::Rook, Side::Black);
      game->m_Board[1][0].emplace(Chess::Piece::Type::Knight, Side::Black);
      game->m_Board[2][0].emplace(Chess::Piece::Type::Bishop, Side::Black);
      game->m_Board[3][0].emplace(Chess::Piece::Type::Queen, Side::Black);
      game->m_Board[4][0].emplace(Chess::Piece::Type::King, Side::Black);
      game->m_Board[5][0].emplace(Chess::Piece::Type::Bishop, Side::Black);
      game->m_Board[6][0].emplace(Chess::Piece::Type::Knight, Side::Black);
      game->m_Board[7][0].emplace(Chess::Piece::Type::Rook, Side::Black);

      game->m_Board[0][1].emplace(Chess::Piece::Type::Pawn, Side::Black);
      game->m_Board[1][1].emplace(Chess::Piece::Type::Pawn, Side::Black);
      game->m_Board[2][1].emplace(Chess::Piece::Type::Pawn, Side::Black);
      game->m_Board[3][1].emplace(Chess::Piece::Type::Pawn, Side::Black);
      game->m_Board[4][1].emplace(Chess::Piece::Type::Pawn, Side::Black);
      game->m_Board[5][1].emplace(Chess::Piece::Type::Pawn, Side::Black);
      game->m_Board[6][1].emplace(Chess::Piece::Type::Pawn, Side::Black);
      game->m_Board[7][1].emplace(Chess::Piece::Type::Pawn, Side::Black);

      game->m_Board[0][6].emplace(Chess::Piece::Type::Pawn, Side::White);
      game->m_Board[1][6].emplace(Chess::Piece::Type::Pawn, Side::White);
      game->m_Board[2][6].emplace(Chess::Piece::Type::Pawn, Side::White);
      game->m_Board[3][6].emplace(Chess::Piece::Type::Pawn, Side::White);
      game->m_Board[4][6].emplace(Chess::Piece::Type::Pawn, Side::White);
      game->m_Board[5][6].emplace(Chess::Piece::Type::Pawn, Side::White);
      game->m_Board[6][6].emplace(Chess::Piece::Type::Pawn, Side::White);
      game->m_Board[7][6].emplace(Chess::Piece::Type::Pawn, Side::White);

      game->m_Board[0][7].emplace(Chess::Piece::Type::Rook, Side::White);
      game->m_Board[1][7].emplace(Chess::Piece::Type::Knight, Side::White);
      game->m_Board[2][7].emplace(Chess::Piece::Type::Bishop, Side::White);
      game->m_Board[3][7].emplace(Chess::Piece::Type::Queen, Side::White);
      game->m_Board[4][7].emplace(Chess::Piece::Type::King, Side::White);
      game->m_Board[5][7].emplace(Chess::Piece::Type::Bishop, Side::White);
      game->m_Board[6][7].emplace(Chess::Piece::Type::Knight, Side::White);
      game->m_Board[7][7].emplace(Chess::Piece::Type::Rook, Side::White);

      game->DrawBoard();
      game->DrawPieces();
      game->HoverCurrentTile();
      Renderer::EndBatch();

      return game;
    }

    void Game::HoverCurrentTile()
    {
      Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * m_CurrentCursorPos.x), -0.7f + (0.2f * m_CurrentCursorPos.y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(15));

      TileInfo info = Game::GetTileInfo(m_CurrentCursorPos, false);

      for (auto pos : info.PossiblePaths)
      {
        Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * pos.x), -0.7f + (0.2f * pos.y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(15));
      }
    }

    bool Game::SelectCurrentTile()
    {
      TileInfo info = Game::GetTileInfo(m_CurrentCursorPos, false);

      if (!info.TilePiece.has_value() || info.PossiblePaths.empty())
        return false;

      m_SelectedTile = info;
      m_CurrentStatus = Status::Selecting;
      return true;
    }

    TileInfo Game::GetTileInfo(glm::ivec2 tile_pos, bool check_test) const
    {
      if (!m_Board[tile_pos.x][tile_pos.y].has_value())
        return {};

      if (m_Board[tile_pos.x][tile_pos.y]->GetSide() != m_Turn && !check_test)
        return {};

      TileInfo tileInfo;
      tileInfo.TilePiece = m_Board[tile_pos.x][tile_pos.y];

      switch (tileInfo.TilePiece->GetType())
      {
      case Piece::Type::Pawn:
      {
        if (m_Turn == Side::White)
        {
          if (tile_pos.y == 6 && tile_pos.y - 2 >= 0 && !m_Board[tile_pos.x][tile_pos.y - 1].has_value() && !m_Board[tile_pos.x][tile_pos.y - 2].has_value())
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y - 2));

          if (tile_pos.y - 1 >= 0)
            if (!m_Board[tile_pos.x][tile_pos.y - 1].has_value())
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y - 1));

          if (tile_pos.x + 1 < 8 && tile_pos.y - 1 >= 0)
            if (m_Board[tile_pos.x + 1][tile_pos.y - 1].has_value() && (m_Board[tile_pos.x + 1][tile_pos.y - 1]->GetSide() != m_Turn || check_test))
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y - 1));

          if (tile_pos.x - 1 >= 0 && tile_pos.y - 1 >= 0)
            if (m_Board[tile_pos.x - 1][tile_pos.y - 1].has_value() && (m_Board[tile_pos.x - 1][tile_pos.y - 1]->GetSide() != m_Turn || check_test))
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y - 1));
        }
        else
        {
          if (tile_pos.y == 1 && tile_pos.y + 2 < 8 && !m_Board[tile_pos.x][tile_pos.y + 1].has_value() && !m_Board[tile_pos.x][tile_pos.y + 2].has_value())
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y + 2));

          if (tile_pos.y + 1 < 8)
            if (!m_Board[tile_pos.x][tile_pos.y + 1].has_value())
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y + 1));

          if (tile_pos.x + 1 < 8 && tile_pos.y + 1 < 8)
            if (m_Board[tile_pos.x + 1][tile_pos.y + 1].has_value() && (m_Board[tile_pos.x + 1][tile_pos.y + 1]->GetSide() != m_Turn || check_test))
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y + 1));

          if (tile_pos.x - 1 >= 0 && tile_pos.y + 1 < 8)
            if (m_Board[tile_pos.x - 1][tile_pos.y + 1].has_value() && (m_Board[tile_pos.x - 1][tile_pos.y + 1]->GetSide() != m_Turn || check_test))
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y + 1));
        }
        break;
      }
      case Piece::Type::Rook:
      {
        for (int32_t x = tile_pos.x + 1; x < 8; x++)
        {
          if (m_Board[x][tile_pos.y].has_value())
          {
            if (m_Board[x][tile_pos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
        }

        for (int32_t x = tile_pos.x - 1; x >= 0; x--)
        {
          if (m_Board[x][tile_pos.y].has_value())
          {
            if (m_Board[x][tile_pos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
        }

        for (int32_t y = tile_pos.y + 1; y < 8; y++)
        {
          if (m_Board[tile_pos.x][y].has_value())
          {
            if (m_Board[tile_pos.x][y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
        }

        for (int32_t y = tile_pos.y - 1; y >= 0; y--)
        {
          if (m_Board[tile_pos.x][y].has_value())
          {
            if (m_Board[tile_pos.x][y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
        }

        break;
      }
      case Piece::Type::Knight:
      {
        glm::ivec2 tileKnightNextPos = tile_pos;

        tileKnightNextPos.x += 2;
        tileKnightNextPos.y += 1;

        if (tileKnightNextPos.x < 8 && tileKnightNextPos.y < 8)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        }

        tileKnightNextPos = tile_pos;

        tileKnightNextPos.x += 2;
        tileKnightNextPos.y -= 1;

        if (tileKnightNextPos.x < 8 && tileKnightNextPos.y >= 0)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        }

        tileKnightNextPos = tile_pos;

        tileKnightNextPos.x -= 2;
        tileKnightNextPos.y += 1;

        if (tileKnightNextPos.x >= 0 && tileKnightNextPos.y < 8)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        }

        tileKnightNextPos = tile_pos;

        tileKnightNextPos.x -= 2;
        tileKnightNextPos.y -= 1;

        if (tileKnightNextPos.x >= 0 && tileKnightNextPos.y >= 0)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        } 

        tileKnightNextPos = tile_pos;

        tileKnightNextPos.x += 1;
        tileKnightNextPos.y += 2;

        if (tileKnightNextPos.x < 8 && tileKnightNextPos.y < 8)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        }

        tileKnightNextPos = tile_pos;

        tileKnightNextPos.x += 1;
        tileKnightNextPos.y -= 2;

        if (tileKnightNextPos.x < 8 && tileKnightNextPos.y >= 0)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        }

        tileKnightNextPos = tile_pos;

        tileKnightNextPos.x -= 1;
        tileKnightNextPos.y += 2;

        if (tileKnightNextPos.x >= 0 && tileKnightNextPos.y < 8)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        }

        tileKnightNextPos = tile_pos;

        tileKnightNextPos.x -= 1;
        tileKnightNextPos.y -= 2;

        if (tileKnightNextPos.x >= 0 && tileKnightNextPos.y >= 0)
        {
          if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y].has_value())
          {
            if (m_Board[tileKnightNextPos.x][tileKnightNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileKnightNextPos);
          }
          else
            tileInfo.PossiblePaths.push_back(tileKnightNextPos);
        }

        break;
      }
      case Piece::Type::Bishop:
      {
        glm::ivec2 tileBishopNextPos = tile_pos;

        tileBishopNextPos.x++;
        tileBishopNextPos.y++;

        while (tileBishopNextPos.x < 8 && tileBishopNextPos.y < 8)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x++;
          tileBishopNextPos.y++;
        }

        tileBishopNextPos = tile_pos;

        tileBishopNextPos.x++;
        tileBishopNextPos.y--;

        while (tileBishopNextPos.x < 8 && tileBishopNextPos.y >= 0)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x++;
          tileBishopNextPos.y--;
        }

        tileBishopNextPos = tile_pos;

        tileBishopNextPos.x--;
        tileBishopNextPos.y--;

        while (tileBishopNextPos.x >= 0 && tileBishopNextPos.y >= 0)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x--;
          tileBishopNextPos.y--;
        }

        tileBishopNextPos = tile_pos;

        tileBishopNextPos.x--;
        tileBishopNextPos.y++;

        while (tileBishopNextPos.x >= 0 && tileBishopNextPos.y < 8)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x--;
          tileBishopNextPos.y++;
        }

        break;
      }
      case Piece::Type::King:
      {
        if (tile_pos.x + 1 < 8)
        {
          if (m_Board[tile_pos.x + 1][tile_pos.y].has_value() && (m_Board[tile_pos.x + 1][tile_pos.y]->GetSide() != m_Turn || check_test))
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y));
          else if (!m_Board[tile_pos.x + 1][tile_pos.y].has_value())
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y));

          if (tile_pos.y + 1 < 8)
          {
            if (m_Board[tile_pos.x + 1][tile_pos.y + 1].has_value() && (m_Board[tile_pos.x + 1][tile_pos.y + 1]->GetSide() != m_Turn || check_test))
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y + 1));
            else if (!m_Board[tile_pos.x + 1][tile_pos.y + 1].has_value())
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y + 1));
          }

          if (tile_pos.y - 1 >= 0)
          {
            if (m_Board[tile_pos.x + 1][tile_pos.y - 1].has_value() && (m_Board[tile_pos.x + 1][tile_pos.y - 1]->GetSide() != m_Turn || check_test))
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y - 1));
            else if (!m_Board[tile_pos.x + 1][tile_pos.y - 1].has_value())
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x + 1, tile_pos.y - 1));
          }
        }
        if (tile_pos.x - 1 >= 0)
        {
          if (m_Board[tile_pos.x - 1][tile_pos.y].has_value() && (m_Board[tile_pos.x - 1][tile_pos.y]->GetSide() != m_Turn || check_test))
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y));
          else if (!m_Board[tile_pos.x - 1][tile_pos.y].has_value())
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y));

          if (tile_pos.y + 1 < 8)
          {
            if (m_Board[tile_pos.x - 1][tile_pos.y + 1].has_value() && (m_Board[tile_pos.x - 1][tile_pos.y + 1]->GetSide() != m_Turn || check_test))
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y + 1));
            else if (!m_Board[tile_pos.x - 1][tile_pos.y + 1].has_value())
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y + 1));
          }

          if (tile_pos.y - 1 >= 0)
          {
            if (m_Board[tile_pos.x - 1][tile_pos.y - 1].has_value() && (m_Board[tile_pos.x - 1][tile_pos.y - 1]->GetSide() != m_Turn || check_test))
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y - 1));
            else if (!m_Board[tile_pos.x - 1][tile_pos.y - 1].has_value())
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x - 1, tile_pos.y - 1));
          }
        }
        if (tile_pos.y + 1 < 8)
        {
          if (m_Board[tile_pos.x][tile_pos.y + 1].has_value() && (m_Board[tile_pos.x][tile_pos.y + 1]->GetSide() != m_Turn || check_test))
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y + 1));
          else if (!m_Board[tile_pos.x][tile_pos.y + 1].has_value())
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y + 1));
        }
        if (tile_pos.y - 1 >= 0)
        {
          if (m_Board[tile_pos.x][tile_pos.y - 1].has_value() && (m_Board[tile_pos.x][tile_pos.y - 1]->GetSide() != m_Turn || check_test))
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y - 1));
          else if (!m_Board[tile_pos.x][tile_pos.y - 1].has_value())
            tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, tile_pos.y - 1));
        }
        break;
      }
      case Piece::Type::Queen:
      {
        glm::ivec2 tileBishopNextPos = tile_pos;

        tileBishopNextPos.x++;
        tileBishopNextPos.y++;

        while (tileBishopNextPos.x < 8 && tileBishopNextPos.y < 8)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x++;
          tileBishopNextPos.y++;
        }

        tileBishopNextPos = tile_pos;

        tileBishopNextPos.x++;
        tileBishopNextPos.y--;

        while (tileBishopNextPos.x < 8 && tileBishopNextPos.y >= 0)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x++;
          tileBishopNextPos.y--;
        }

        tileBishopNextPos = tile_pos;

        tileBishopNextPos.x--;
        tileBishopNextPos.y--;

        while (tileBishopNextPos.x >= 0 && tileBishopNextPos.y >= 0)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x--;
          tileBishopNextPos.y--;
        }

        tileBishopNextPos = tile_pos;

        tileBishopNextPos.x--;
        tileBishopNextPos.y++;

        while (tileBishopNextPos.x >= 0 && tileBishopNextPos.y < 8)
        {
          if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y].has_value())
          {
            if (m_Board[tileBishopNextPos.x][tileBishopNextPos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(tileBishopNextPos);
            break;
          }

          tileInfo.PossiblePaths.push_back(tileBishopNextPos);

          tileBishopNextPos.x--;
          tileBishopNextPos.y++;
        }

        for (int32_t x = tile_pos.x + 1; x < 8; x++)
        {
          if (m_Board[x][tile_pos.y].has_value())
          {
            if (m_Board[x][tile_pos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
        }

        for (int32_t x = tile_pos.x - 1; x >= 0; x--)
        {
          if (m_Board[x][tile_pos.y].has_value())
          {
            if (m_Board[x][tile_pos.y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(x, tile_pos.y));
        }

        for (int32_t y = tile_pos.y + 1; y < 8; y++)
        {
          if (m_Board[tile_pos.x][y].has_value())
          {
            if (m_Board[tile_pos.x][y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
        }

        for (int32_t y = tile_pos.y - 1; y >= 0; y--)
        {
          if (m_Board[tile_pos.x][y].has_value())
          {
            if (m_Board[tile_pos.x][y]->GetSide() != m_Turn || check_test)
              tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
            break;
          }
          tileInfo.PossiblePaths.push_back(glm::ivec2(tile_pos.x, y));
        }

        break;
      }
      }

      return tileInfo;
    }

    std::optional<glm::ivec2> Game::FindKing(Side side) const
    {
      for (int y = 0; y < 8; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          const auto& pieceOpt = m_Board[x][y];
          if (pieceOpt.has_value())
          {
            const Piece& piece = *pieceOpt;
            if (piece.GetSide() == side && piece.GetType() == Piece::Type::King)
            {
              return glm::ivec2{ x, y };
            }
          }
        }
      }
      return std::nullopt;
    }

    bool Game::IsSquareAttacked(glm::ivec2 square, Side attacker) const
    {
      // Loop through all pieces of the attacker side
      for (int y = 0; y < 8; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          auto pieceOpt = m_Board[x][y];
          if (!pieceOpt.has_value()) continue;

          const Piece& piece = pieceOpt.value();
          if (piece.GetSide() != attacker) continue;

          // Get pseudo-legal moves for this piece
          TileInfo info = Game::GetTileInfo({ x, y }, true);
          for (auto& move : info.PossiblePaths)
          {
            if (move == square)
              return true;
          }
        }
      }
      return false;
    }

    bool Game::IsInCheck(Side side) const
    {
      std::optional<glm::ivec2> kingPos = Game::FindKing(side);

      if (kingPos == std::nullopt)
        return false;

      Side opponent = (side == Side::White) ? Side::Black : Side::White;
      return Game::IsSquareAttacked(kingPos.value(), opponent);
    }

    bool Game::IsCheckmate(Side side)
    {
      if (!Game::IsInCheck(side))
        return false; // Not in check, so can't be checkmate

      // Try all possible moves
      for (int y = 0; y < 8; y++)
      {
        for (int x = 0; x < 8; x++)
        {
          auto pieceOpt = m_Board[x][y];
          if (!pieceOpt.has_value()) continue;

          const Piece& piece = pieceOpt.value();
          if (piece.GetSide() != side) continue;

          TileInfo info = Game::GetTileInfo({ x, y }, false);
          for (auto& move : info.PossiblePaths)
          {
            // Copy board to simulate
            auto backup = m_Board;
            m_Board[move.x][move.y] = m_Board[x][y];
            m_Board[x][y].reset();

            bool stillInCheck = Game::IsInCheck(side);

            // Restore board
            m_Board = backup;

            if (!stillInCheck)
              return false; // Found a move that avoids check
          }
        }
      }

      return true; // No escape, checkmate
    }

    void Game::DrawBoard()
    {
      Renderer::DrawImage(glm::vec2(-0.793f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(24)); // Corner
      Renderer::DrawImage(glm::vec2(-0.7f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(16)); // Letter a (Border)
      Renderer::DrawImage(glm::vec2(-0.5f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(17)); // Letter b (Border)
      Renderer::DrawImage(glm::vec2(-0.3f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(18)); // Letter c (Border)
      Renderer::DrawImage(glm::vec2(-0.1f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(19)); // Letter d (Border)
      Renderer::DrawImage(glm::vec2(0.1f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(20)); // Letter e (Border)
      Renderer::DrawImage(glm::vec2(0.3f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(21)); // Letter f (Border)
      Renderer::DrawImage(glm::vec2(0.5f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(22)); // Letter g (Border)
      Renderer::DrawImage(glm::vec2(0.7f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(23)); // Letter h (Border)
      Renderer::DrawImage(glm::vec2(0.9f, -0.9f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(24)); // Corner

      Renderer::DrawImage(glm::vec2(-0.793f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(25)); // Number 1 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.5f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.3f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.1f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.1f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.3f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.5f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.7f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.9f, -0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(25)); // Number 1 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(26)); // Number 2 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.5f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.3f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.1f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.1f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.3f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.5f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.7f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.9f, -0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(26)); // Number 2 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(27)); // Number 3 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.5f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.3f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.1f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.1f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.3f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.5f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.7f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.9f, -0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(27)); // Number 3 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(28)); // Number 4 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.5f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.3f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.1f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.1f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.3f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.5f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.7f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.9f, -0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(28)); // Number 4 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(29)); // Number 5 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.5f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.3f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.1f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.1f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.3f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.5f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.7f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.9f, 0.1f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(29)); // Number 5 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(30)); // Number 6 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.5f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.3f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.1f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.1f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.3f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.5f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.7f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.9f, 0.3f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(30)); // Number 6 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(31)); // Number 7 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.5f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.3f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.1f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.1f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.3f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.5f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.7f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.9f, 0.5f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(31)); // Number 7 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(32)); // Number 8 (Border)
      Renderer::DrawImage(glm::vec2(-0.7f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.5f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(-0.3f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(-0.1f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.1f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.3f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.5f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(0)); // Black
      Renderer::DrawImage(glm::vec2(0.7f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(1)); // White
      Renderer::DrawImage(glm::vec2(0.9f, 0.7f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(32)); // Number 8 (Border)

      Renderer::DrawImage(glm::vec2(-0.793f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(24)); // Corner
      Renderer::DrawImage(glm::vec2(-0.7f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(16)); // Letter a (Border)
      Renderer::DrawImage(glm::vec2(-0.5f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(17)); // Letter b (Border)
      Renderer::DrawImage(glm::vec2(-0.3f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(18)); // Letter c (Border)
      Renderer::DrawImage(glm::vec2(-0.1f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(19)); // Letter d (Border)
      Renderer::DrawImage(glm::vec2(0.1f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(20)); // Letter e (Border)
      Renderer::DrawImage(glm::vec2(0.3f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(21)); // Letter f (Border)
      Renderer::DrawImage(glm::vec2(0.5f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(22)); // Letter g (Border)
      Renderer::DrawImage(glm::vec2(0.7f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(23)); // Letter h (Border)
      Renderer::DrawImage(glm::vec2(0.9f, 0.793f), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(24)); // Corner
    }

    void Game::DrawPieces()
    {
      for (int32_t y = 0; y < 8; y++)
      {
        for (int32_t x = 0; x < 8; x++)
        {
          std::optional<Piece> piece = m_Board[x][y];
          if (piece.has_value())
          {
            if (piece->GetSide() == Side::White)
            {
              switch (piece->GetType())
              {
              case Chess::Piece::Type::Pawn:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(3));
                break;
              }
              case Chess::Piece::Type::Rook:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(4));
                break;
              }
              case Chess::Piece::Type::Knight:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(2));
                break;
              }
              case Chess::Piece::Type::Bishop:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(5));
                break;
              }
              case Chess::Piece::Type::King:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(6));
                break;
              }
              case Chess::Piece::Type::Queen:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(7));
                break;
              }
              }
            }
            else
            {
              switch (piece->GetType())
              {
              case Chess::Piece::Type::Pawn:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(9));
                break;
              }
              case Chess::Piece::Type::Rook:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(10));
                break;
              }
              case Chess::Piece::Type::Knight:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(8));
                break;
              }
              case Chess::Piece::Type::Bishop:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(11));
                break;
              }
              case Chess::Piece::Type::King:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(12));
                break;
              }
              case Chess::Piece::Type::Queen:
              {
                Renderer::DrawImage(glm::vec2(-0.7f + (0.2f * x), -0.7f + (0.2f * y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(13));
                break;
              }
              }
            }
          }
        }
      }
    }

    void Game::OnKeyPress(KeyCode key, bool repeat)
    {
      switch (m_CurrentStatus)
      {
      case Game::Status::Hovering:
      {
        if (repeat)
          return;

        glm::ivec2 newPos = m_CurrentCursorPos;

        if (key == Key::Enter)
        {
          if (!Game::SelectCurrentTile())
            return;

          Renderer::ResetBatch();
          Game::DrawBoard();
          Game::DrawPieces();
          Game::HoverCurrentTile();

          if (Game::IsInCheck(m_Turn))
          {
            std::optional<glm::ivec2> kingPos = Game::FindKing(m_Turn);
            Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * kingPos->x), -0.7f + (0.2f * kingPos->y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(14));
          }

          Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * m_SelectedTile.PossiblePaths[m_SelectedTileIndex].x), -0.7f + (0.2f * m_SelectedTile.PossiblePaths[m_SelectedTileIndex].y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(14));

          Renderer::EndBatch();
          return;
        }

        if (key == Key::Up)
        {
          newPos.y--;
        }
        else if (key == Key::Down)
        {
          newPos.y++;
        }
        else if (key == Key::Right)
        {
          newPos.x++;
        }
        else if (key == Key::Left)
        {
          newPos.x--;
        }

        newPos.x = std::clamp(newPos.x, 0, 7);
        newPos.y = std::clamp(newPos.y, 0, 7);

        m_CurrentCursorPos = newPos;
        Renderer::ResetBatch();
        Game::DrawBoard();
        Game::DrawPieces();
        Game::HoverCurrentTile();
        if (Game::IsInCheck(m_Turn))
        {
          std::optional<glm::ivec2> kingPos = Game::FindKing(m_Turn);
          Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * kingPos->x), -0.7f + (0.2f * kingPos->y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(14));
        }
        Renderer::EndBatch();
        break;
      }
      case Game::Status::Selecting:
      {
        if (key == Key::Escape)
        {
          m_CurrentStatus = Status::Hovering;
          Renderer::ResetBatch();
          Game::DrawBoard();
          Game::DrawPieces();
          Game::HoverCurrentTile();
          Renderer::EndBatch();
          m_SelectedTileIndex = 0;
          return;
        }

        if (repeat || !m_SelectedTile.TilePiece.has_value())
          return;

        if (key == Key::Up || key == Key::Right)
        {
          m_SelectedTileIndex++;
          if (m_SelectedTileIndex > m_SelectedTile.PossiblePaths.size() - 1)
            m_SelectedTileIndex = 0;
        }
        else if (key == Key::Down || key == Key::Left)
        {
          m_SelectedTileIndex--;
          if (m_SelectedTileIndex < 0)
            m_SelectedTileIndex = m_SelectedTile.PossiblePaths.size() - 1;
        }
        else if (key == Key::Enter)
        {
          if (Game::IsInCheck(m_Turn))
          {
            auto backup = m_Board;
            m_Board[m_SelectedTile.PossiblePaths[m_SelectedTileIndex].x][m_SelectedTile.PossiblePaths[m_SelectedTileIndex].y] = m_Board[m_CurrentCursorPos.x][m_CurrentCursorPos.y];
            m_Board[m_CurrentCursorPos.x][m_CurrentCursorPos.y].reset();

            if (Game::IsInCheck(m_Turn))
            {
              m_Board = backup;
              break;
            }
          }
          else
          {
            m_Board[m_SelectedTile.PossiblePaths[m_SelectedTileIndex].x][m_SelectedTile.PossiblePaths[m_SelectedTileIndex].y] = m_Board[m_CurrentCursorPos.x][m_CurrentCursorPos.y];
            m_Board[m_CurrentCursorPos.x][m_CurrentCursorPos.y].reset();
          }

          m_SelectedTileIndex = 0;
          m_CurrentStatus = Status::Hovering;
          m_Turn = (m_Turn == Side::White) ? Side::Black : Side::White;
          m_CurrentCursorPos = (m_Turn == Side::White) ? glm::ivec2(7, 7) : glm::ivec2(0, 0);

          if (Game::IsCheckmate(m_Turn))
          {
            YK_INFO("Game over for side: {}", (m_Turn == Side::White) ? "White" : "Black");
            break;
          }

          Renderer::ResetBatch();
          Game::DrawBoard();
          Game::DrawPieces();
          Game::HoverCurrentTile();
          if (Game::IsInCheck(m_Turn))
          {
            std::optional<glm::ivec2> kingPos = Game::FindKing(m_Turn);
            Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * kingPos->x), -0.7f + (0.2f * kingPos->y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(14));
          }
          Renderer::EndBatch();
          break;
        }

        Renderer::ResetBatch();
        Game::DrawBoard();
        Game::DrawPieces();
        Game::HoverCurrentTile();
        if (Game::IsInCheck(m_Turn))
        {
          std::optional<glm::ivec2> kingPos = Game::FindKing(m_Turn);
          Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * kingPos->x), -0.7f + (0.2f * kingPos->y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(14));
        }
        Renderer::DrawImage(glm::vec2(-0.7 + (0.2f * m_SelectedTile.PossiblePaths[m_SelectedTileIndex].x), -0.7f + (0.2f * m_SelectedTile.PossiblePaths[m_SelectedTileIndex].y)), glm::vec2(0.2f, 0.2f), 1, m_ChessAtlas->GetSubTexture(14));

        Renderer::EndBatch();

        break;
      }
      }
    }

    void Game::OnMouseButtonPress(MouseCode button)
    {
      switch (button)
      {
      case Mouse::ButtonLeft:
      {
        Renderer::UpdateIDFramebuffer();

        glm::vec2 mousePos = Input::GetMousePosition();
        glm::uvec2 winSize = WindowManager::GetWindowSize();
        glm::uvec2 fbSize = WindowManager::GetFramebufferSize();

        float scaleX = static_cast<float>(fbSize.x) / static_cast<float>(winSize.x);
        float scaleY = static_cast<float>(fbSize.y) / static_cast<float>(winSize.y);

        int32_t pixelX = std::clamp(static_cast<int>(mousePos.x * scaleX), 0, static_cast<int32_t>(fbSize.x) - 1);
        int32_t pixelY = std::clamp(static_cast<int>(mousePos.y * scaleY), 0, static_cast<int32_t>(fbSize.y) - 1);

        pixelY = fbSize.y - 1 - pixelY;

        uint32_t id = Renderer::GetPositionID(glm::uvec2(pixelX, pixelY));

        YK_INFO("ID {}", id);

        break;
      }
      }
    }

  }
}