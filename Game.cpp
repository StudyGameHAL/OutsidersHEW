#include "Pch.h"
#include "Game.h"
#include "SceneGame.h"

Game* Game::m_Instance{ nullptr };

Game::Game()
{
	m_Instance = this;

	// TODO: Render / Input ...‰Šú‰»
	SetNextScene<SceneGame>();
}

Game::~Game()
{
	delete m_Scene;
	delete m_SceneNext;
}

void Game::Update()
{
	UpdateSceneTransform();
	// TODO: Input
	m_Scene->Update();
}

void Game::Draw()
{
	m_Scene->Draw();
}

void Game::UpdateSceneTransform()
{
	if (!m_SceneNext)
	{
		return;
	}
	if (m_Scene)
	{
		m_Scene->Finalize();
		delete m_Scene;
	}
	m_Scene = m_SceneNext;
	m_SceneNext = nullptr;
}
