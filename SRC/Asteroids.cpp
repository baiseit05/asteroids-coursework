#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include <algorithm>
#include "ScoreKeeper.h"
#include "BonusExtraLife.h"
#include "BonusInvulnerability.h"
#include <iomanip>




// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv), mMenuDisplay(new GameDisplay(400, 400))

{
	mMenuDisplay = new GameDisplay(400, 400);
	mLevel = 0;
	mAsteroidCount = 0;

	// Set initial state to menu
	currentState = MENU;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////


void Asteroids::SetupInputListeners()
{
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);
	mGameWindow->AddKeyboardListener(thisPtr);
	mGameWorld->AddListener(thisPtr.get());
}


void Asteroids::Start()
{
	

	SetupInputListeners();
	

	// Light, animation, and GUI setup can still happen
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("bonus_life", 64, 1024, 64, 64, "asteroid_bonus_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("bonus_invuln", 64, 1024, 64, 64, "enemy_fs.png");




	CreateBackgroundAsteroids(10); // new floating background asteroids

	// Hide GUI elements by default for menu
	if (mScoreLabel) mScoreLabel->SetVisible(false);
	if (mLivesLabel) mLivesLabel->SetVisible(false);
	if (mGameOverLabel) mGameOverLabel->SetVisible(false);

	mGameWindow->SetDisplay(mMenuDisplay);
	
	CreateMenuGUI();

	// Game will start from menu – not here
	

	GameSession::Start();
	

}





/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////
void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (currentState == MENU)
	{
		if (key == 's' || key == 'S') {
			StartGame();
		}
		else if (key == 'i' || key == 'I') {
			ShowInstructions();
		}
		else if (key == 'h' || key == 'H') {
			ShowHighScores();
		}

		else if (key == 'd' || key == 'D') {
			difficultyEnabled = !difficultyEnabled;
			

			// Update the on-screen difficulty label
			if (mDifficultyLabel) {
				std::string diffText = "Difficulty: ";
				diffText += (difficultyEnabled ? "Powerups Enabled" : "Powerups Disabled");
				mDifficultyLabel->SetText(diffText);
			}
		}
	}
	else if (currentState == INSTRUCTIONS)
	{
		if (key == 'm' || key == 'M') {
			// Go back to Menu
			currentState = MENU;

			// Hide instructions text
			if (mInstructionsTextLabel) {
				mInstructionsTextLabel->SetVisible(false);
				if (mControlsLabel) mControlsLabel->SetVisible(false);
				if (mAvoidLabel) mAvoidLabel->SetVisible(false);
				if (mSurviveLabel) mSurviveLabel->SetVisible(false);
				if (mReturnLabel) mReturnLabel->SetVisible(false);
				if (mPowerupLabel) mPowerupLabel->SetVisible(false);

			}

			// Show main menu labels again
			if (mMenuTitleLabel) mMenuTitleLabel->SetVisible(true);
			if (mStartGameLabel) mStartGameLabel->SetVisible(true);
			if (mInstructionsLabel) mInstructionsLabel->SetVisible(true);
			if (mDifficultyLabel) mDifficultyLabel->SetVisible(true);
			if (mHighScoreLabel) mHighScoreLabel->SetVisible(true);

		}
	}
	else if (currentState == PLAYING)
	{
		if (key == ' ') {
			mSpaceship->Shoot();
		}

	}

	else if (currentState == HIGH_SCORE)
	{
		if (key == 'm' || key == 'M') {
			// Go back to Menu
			currentState = MENU;
			HideHighScoreLabels();

			// Show menu labels
			if (mMenuTitleLabel) mMenuTitleLabel->SetVisible(true);
			if (mStartGameLabel) mStartGameLabel->SetVisible(true);
			if (mInstructionsLabel) mInstructionsLabel->SetVisible(true);
			if (mDifficultyLabel) mDifficultyLabel->SetVisible(true);
			if (mHighScoreLabel) mHighScoreLabel->SetVisible(true);
		}
	}

	else if (currentState == ENTER_NAME)
	{
		if (key == 13) { // Enter key
			int score = mScoreKeeper.GetScore();  
			AddHighScore(mNameInput, score);      
			ShowHighScores();
		}
		else if (key == 8) { // Backspace
			if (!mNameInput.empty()) mNameInput.pop_back();
		}
		else {
			mNameInput += key;
		}

		if (mPlayerNameLabel) {
			mPlayerNameLabel->SetText(mNameInput);
		}
	}


}



void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////



void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0)
		{
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
	else if (object->GetType() == GameObjectType("BonusExtraLife"))
	{
		mPlayer.AddLife();
		std::cout << "Bonus collected! Extra life added.\n";

		// Update the lives label
		std::ostringstream msg_stream;
		msg_stream << "Lives: " << mPlayer.GetLives();  
		std::string lives_msg = msg_stream.str();
		mLivesLabel->SetText(lives_msg);
	}


}




// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}


	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(false);
		currentState = ENTER_NAME;
		mNameInput.clear();

		// Hide menu options
		if (mMenuTitleLabel) mMenuTitleLabel->SetVisible(false);
		if (mStartGameLabel) mStartGameLabel->SetVisible(false);
		if (mInstructionsLabel) mInstructionsLabel->SetVisible(false);
		if (mDifficultyLabel) mDifficultyLabel->SetVisible(false);
		if (mHighScoreLabel) mHighScoreLabel->SetVisible(false);

		// Create label: "Enter Your Name:"
		if (!mEnterNameLabel) {
			mEnterNameLabel = make_shared<GUILabel>("Enter Your Name:");
			mEnterNameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
			mEnterNameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
			mMenuDisplay->GetContainer()->AddComponent(
				static_pointer_cast<GUIComponent>(mEnterNameLabel),
				GLVector2f(0.5f, 0.7f)
			);
		}

		if (!mPlayerNameLabel) {
			mPlayerNameLabel = make_shared<GUILabel>("");
			mPlayerNameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
			mPlayerNameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
			mMenuDisplay->GetContainer()->AddComponent(
				static_pointer_cast<GUIComponent>(mPlayerNameLabel),
				GLVector2f(0.5f, 0.6f)
			);
		}

		mEnterNameLabel->SetVisible(true);
		mPlayerNameLabel->SetVisible(true);

		mGameWindow->SetDisplay(mMenuDisplay);
	}




}

void Asteroids::StartGame()
{
	// Create the main game display
	mGameDisplay = new GameDisplay(800, 600);

	// Remove floating background asteroids from the menu
	for (auto& asteroid : mBackgroundAsteroids) {
		if (asteroid) mGameWorld->FlagForRemoval(asteroid);
	}
	mBackgroundAsteroids.clear();

	currentState = PLAYING;
	mGameWindow->SetDisplay(mGameDisplay);

	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add game listeners
	//mGameWorld->AddListener(thisPtr.get());
	mGameWorld->AddListener(&mScoreKeeper);
	mScoreKeeper.AddListener(thisPtr);
	mGameWorld->AddListener(&mPlayer);
	mPlayer.AddListener(thisPtr);

	// Create spaceship and asteroids
	mGameWorld->AddObject(CreateSpaceship());
	CreateAsteroids(10);
	CreateGUI();

	// Reset game state
	mScoreKeeper.ResetScore();
	mPlayer.SetLives(3);
	mLevel = 0;

	// Make score/lives visible once game starts
	mScoreLabel->SetVisible(true);
	mLivesLabel->SetVisible(true);

	// Reset name input from previous game
	mNameInput.clear();
	if (mPlayerNameLabel) mPlayerNameLabel->SetText("");

	// Spawn bonus if powerups are enabled
	if (difficultyEnabled)
	{
		auto bonus = CreateBonusExtraLife();

		// Set random position and velocity
		float x = (rand() % 400) - 200;
		float y = (rand() % 400) - 200;
		float vx = ((rand() % 10) - 5) / 20.0f;
		float vy = ((rand() % 10) - 5) / 20.0f;

		bonus->SetPosition(GLVector3f(x, y, 0));
		bonus->SetVelocity(GLVector3f(vx, vy, 0));

		mGameWorld->AddObject(bonus);

		// Invulnerability Bonus
		auto invulnBonus = CreateBonusInvulnerability();
		float x2 = (rand() % 400) - 200;
		float y2 = (rand() % 400) - 200;
		float vx2 = ((rand() % 10) - 5) / 20.0f;
		float vy2 = ((rand() % 10) - 5) / 20.0f;
		invulnBonus->SetPosition(GLVector3f(x2, y2, 0));
		invulnBonus->SetVelocity(GLVector3f(vx2, vy2, 0));
		mGameWorld->AddObject(invulnBonus);


	}

}







void Asteroids::ShowInstructions()
{
	currentState = INSTRUCTIONS;
	mGameWindow->SetDisplay(mMenuDisplay); // Keep using the menu display

	// Hide menu
	if (mMenuTitleLabel) mMenuTitleLabel->SetVisible(false);
	if (mStartGameLabel) mStartGameLabel->SetVisible(false);
	if (mInstructionsLabel) mInstructionsLabel->SetVisible(false);
	if (mDifficultyLabel) mDifficultyLabel->SetVisible(false);
	if (mHighScoreLabel) mHighScoreLabel->SetVisible(false);


	if (!mInstructionsTextLabel)
	{
		// Create all labels once
		mInstructionsTextLabel = make_shared<GUILabel>("HOW TO PLAY");
		mInstructionsTextLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mInstructionsTextLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mMenuDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(mInstructionsTextLabel),
			GLVector2f(0.5f, 0.8f)
		);

		mControlsLabel = make_shared<GUILabel>("Use arrow keys to move | Press SPACE to shoot");
		mControlsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mControlsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mMenuDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(mControlsLabel),
			GLVector2f(0.5f, 0.7f)
		);

		mAvoidLabel = make_shared<GUILabel>("Avoid or destroy asteroids");
		mAvoidLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mAvoidLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mMenuDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(mAvoidLabel),
			GLVector2f(0.5f, 0.6f)
		);

		mSurviveLabel = make_shared<GUILabel>("Survive as long as you can");
		mSurviveLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mSurviveLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mMenuDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(mSurviveLabel),
			GLVector2f(0.5f, 0.5f)
		);

		mPowerupLabel = make_shared<GUILabel>("Power-Ups: Red = +1 Life, Green = Invulnerable for 10s");
		mPowerupLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mPowerupLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mMenuDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(mPowerupLabel),
			GLVector2f(0.5f, 0.3f) // Below return label
		);


		mReturnLabel = make_shared<GUILabel>("Press 'M' to return to Menu");
		mReturnLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mReturnLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mMenuDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(mReturnLabel),
			GLVector2f(0.5f, 0.4f)
		);
	}

	//  make instruction labels visible when showing instructions
	if (mInstructionsTextLabel) mInstructionsTextLabel->SetVisible(true);
	if (mControlsLabel) mControlsLabel->SetVisible(true);
	if (mAvoidLabel) mAvoidLabel->SetVisible(true);
	if (mSurviveLabel) mSurviveLabel->SetVisible(true);
	if (mPowerupLabel) mPowerupLabel->SetVisible(true);
	if (mReturnLabel) mReturnLabel->SetVisible(true);
}

void Asteroids::HideInstructionLabels()
{
	if (mInstructionsTextLabel) mInstructionsTextLabel->SetVisible(false);
	if (mControlsLabel) mControlsLabel->SetVisible(false);
	if (mAvoidLabel) mAvoidLabel->SetVisible(false);
	if (mSurviveLabel) mSurviveLabel->SetVisible(false);
	if (mPowerupLabel) mPowerupLabel->SetVisible(false);
	if (mReturnLabel) mReturnLabel->SetVisible(false);
}

void Asteroids::ShowHighScores()
{
	currentState = HIGH_SCORE;
	mGameWindow->SetDisplay(mMenuDisplay);

	// Hide name entry labels
	if (mEnterNameLabel) mEnterNameLabel->SetVisible(false);
	if (mPlayerNameLabel) mPlayerNameLabel->SetVisible(false);


	// Hide main menu labels
	if (mMenuTitleLabel) mMenuTitleLabel->SetVisible(false);
	if (mStartGameLabel) mStartGameLabel->SetVisible(false);
	if (mInstructionsLabel) mInstructionsLabel->SetVisible(false);
	if (mDifficultyLabel) mDifficultyLabel->SetVisible(false);
	if (mHighScoreLabel) mHighScoreLabel->SetVisible(false);


	// Clear old high score labels
	HideHighScoreLabels();
	mHighScoreLabels.clear();

	// Title
	auto titleLabel = make_shared<GUILabel>("HIGH SCORES");
	titleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	titleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mMenuDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(titleLabel),
		GLVector2f(0.5f, 0.8f)
	);
	mHighScoreLabels.push_back(titleLabel);

	// Display each high score entry
	float y = 0.7f;
	for (const auto& entry : mHighScores) {
		std::ostringstream msg;
		msg << entry.first << " - " << entry.second;
		auto label = make_shared<GUILabel>(msg.str());
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
		mMenuDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(label),
			GLVector2f(0.5f, y)
		);
		mHighScoreLabels.push_back(label);
		y -= 0.05f;
	}

	// Hint to go back
	auto returnLabel = make_shared<GUILabel>("Press 'M' to return to Menu");
	returnLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	returnLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mMenuDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(returnLabel),
		GLVector2f(0.5f, 0.2f)
	);
	mHighScoreLabels.push_back(returnLabel);
}

void Asteroids::HideHighScoreLabels()
{
	for (auto& label : mHighScoreLabels) {
		if (label) label->SetVisible(false);
	}
}

void Asteroids::AddHighScore(const std::string& name, int score)
{
	mHighScores.push_back(std::make_pair(name, score));

	// Optional: sort high scores highest to lowest
	std::sort(mHighScores.begin(), mHighScores.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
		});

	// Optional: keep only top 10 scores
	if (mHighScores.size() > 10) {
		mHighScores.resize(10);
	}
}










// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}


void Asteroids::CreateBackgroundAsteroids(int count)
{
	for (int i = 0; i < count; ++i)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		if (!anim_ptr) continue; // Skip if animation not found

		shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		sprite->SetLoopAnimation(true);

		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		


		if (!asteroid) continue; // Safety check

		asteroid->SetSprite(sprite);
		asteroid->SetScale(0.2f);

		float x = (rand() % 400) - 200;
		float y = (rand() % 400) - 200;
		float vx = ((rand() % 10) - 5) / 20.0f;
		float vy = ((rand() % 10) - 5) / 20.0f;

		asteroid->SetPosition(GLVector3f(x, y, 0));
		asteroid->SetVelocity(GLVector3f(vx, vy, 0));

		mGameWorld->AddObject(asteroid);

		// Only add non-null asteroids to the tracking list
		mBackgroundAsteroids.push_back(asteroid);
	}
}



//this is not drawing menu
void Asteroids::DrawMenu()
{
	glColor3f(1.0f, 1.0f, 1.0f); // white text
	RenderText(250, 400, GLUT_BITMAP_HELVETICA_18, "ASTEROIDS");
	RenderText(250, 360, GLUT_BITMAP_HELVETICA_12, "Press 'S' to Start Game");
	RenderText(250, 330, GLUT_BITMAP_HELVETICA_12, "Press 'I' for Instructions");
	RenderText(250, 300, GLUT_BITMAP_HELVETICA_12, "Press 'H' for High Scores");
}




void Asteroids::RenderText(float x, float y, void* font, const char* text)
{
	glRasterPos2f(x, y);
	while (*text) {
		glutBitmapCharacter(font, *text++);
	}
}
//this is not drawing menu
void Asteroids::OnWorldUpdated(GameWorld* world)
{
	std::cout << "World is updating..." << std::endl;
	// Draw the menu overlay only in MENU state
	if (currentState == MENU) {
		DrawMenu();
	}
}




void Asteroids::CreateGUI()
{
	if (currentState != PLAYING) return;
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	mInvulnTimerLabel = make_shared<GUILabel>("");
	mInvulnTimerLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> timer_component = static_pointer_cast<GUIComponent>(mInvulnTimerLabel);
	mGameDisplay->GetContainer()->AddComponent(timer_component, GLVector2f(0.5f, 1.0f)); // Center-top
	mInvulnTimerLabel->SetVisible(false);


	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

}


void Asteroids::CreateMenuGUI()
{
	mMenuTitleLabel = make_shared<GUILabel>("ASTEROIDS");
	mMenuTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mMenuTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mMenuDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mMenuTitleLabel),
		GLVector2f(0.5f, 0.7f)
	);

	mStartGameLabel = make_shared<GUILabel>("Press 'S' to Start Game");
	mStartGameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mStartGameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mMenuDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mStartGameLabel),
		GLVector2f(0.5f, 0.6f)
	);

	mInstructionsLabel = make_shared<GUILabel>("Press 'I' for Instructions");
	mInstructionsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mMenuDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mInstructionsLabel),
		GLVector2f(0.5f, 0.5f)
	);

	mDifficultyLabel = make_shared<GUILabel>("Press 'D' to Toggle Difficulty");
	mDifficultyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mMenuDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mDifficultyLabel),
		GLVector2f(0.5f, 0.4f)
	);

	mHighScoreLabel = make_shared<GUILabel>("Press 'H' for High Scores"); // 
	mHighScoreLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mMenuDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mHighScoreLabel),
		GLVector2f(0.5f, 0.3f) // lower
	);
}



void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{

	if (mSpaceship) {
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(mSpaceship->GetPosition());
		explosion->SetRotation(mSpaceship->GetRotation());
		mGameWorld->AddObject(explosion);
	}

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

shared_ptr<GameObject> Asteroids::CreateBonusExtraLife()
{
	shared_ptr<BonusExtraLife> bonus = make_shared<BonusExtraLife>();

	// Safe to use GetThisPtr() now
	bonus->SetBoundingShape(make_shared<BoundingSphere>(bonus->GetThisPtr(), 5.0f));

	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("bonus_life");
	if (anim_ptr)
	{
		shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		sprite->SetLoopAnimation(true);
		bonus->SetSprite(sprite);
		bonus->SetScale(0.1f);
	}

	return bonus;
}

shared_ptr<GameObject> Asteroids::CreateBonusInvulnerability()
{
	shared_ptr<BonusInvulnerability> bonus = make_shared<BonusInvulnerability>();

	// Assign bounding shape for collision detection
	bonus->SetBoundingShape(make_shared<BoundingSphere>(bonus->GetThisPtr(), 5.0f));

	// Assign sprite and scale
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("bonus_invuln");
	if (anim_ptr)
	{
		shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		sprite->SetLoopAnimation(true);
		bonus->SetSprite(sprite);
		bonus->SetScale(0.1f);
	}

	return bonus;
}









