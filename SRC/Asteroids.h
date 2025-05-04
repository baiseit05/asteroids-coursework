#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include <vector>

class GameObject;
class Spaceship;
class GUILabel;

// game state enum to manage which screen is being shown
enum GameState {
	MENU,
	PLAYING,
	GAME_OVER,
	INSTRUCTIONS,
	HIGH_SCORE,
	ENTER_NAME
};

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);
	



	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	
	void OnWorldUpdated(GameWorld* world);

	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	GameDisplay* mMenuDisplay;
	GameDisplay* mGameDisplay;
	shared_ptr<GUILabel> mMenuTitleLabel;
	shared_ptr<GUILabel> mStartGameLabel;
	shared_ptr<GUILabel> mInstructionsLabel;
	shared_ptr<GUILabel> mDifficultyLabel;
	shared_ptr<GUILabel> mInstructionsTextLabel;
	shared_ptr<GUILabel> mHighScoreLabel;
	shared_ptr<GUILabel> mControlsLabel;
	shared_ptr<GUILabel> mAvoidLabel;
	shared_ptr<GUILabel> mSurviveLabel;
	shared_ptr<GUILabel> mReturnLabel;
	shared_ptr<GUILabel> mEnterNameLabel;
	shared_ptr<GUILabel> mPlayerNameLabel;
	shared_ptr<GUILabel> mInvulnTimerLabel;
	shared_ptr<GUILabel> mPowerupLabel;



	std::string mNameInput; // stores typed characters




	std::vector<shared_ptr<GameObject>> mBackgroundAsteroids;



	uint mLevel;
	uint mAsteroidCount;

	// Store the current game state (e.g. MENU or PLAYING)
	GameState currentState;

	void StartGame();
	void SetupInputListeners();

	void ShowInstructions();
	void HideInstructionLabels();


	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateMenuGUI();

	void CreateAsteroids(const uint num_asteroids);
	void CreateBackgroundAsteroids(int count);
	shared_ptr<GameObject> CreateExplosion();

	shared_ptr<GameObject> CreateBonusExtraLife(); // NEW

	shared_ptr<GameObject> CreateBonusInvulnerability();

	bool difficultyEnabled = false; // default 


	// NEW DRAW METHODS FOR MENU

	

	
	void DrawMenu(); // displays the text menu overlay
	void RenderText(float x, float y, void* font, const char* text); // text drawing helper
	
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;

	std::vector<std::pair<std::string, int>> mHighScores; // stores (name, score) pairs
	std::vector<std::shared_ptr<GUILabel>> mHighScoreLabels; // GUI labels for high score entries

	void ShowHighScores();
	void HideHighScoreLabels();
	void AddHighScore(const std::string& name, int score);


};

#endif