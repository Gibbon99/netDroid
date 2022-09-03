#include <iostream>
#include "classes/com_networkState.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Class constructor - create mutex to allow safe access to networkState variables from threads
networkState::networkState ()
//----------------------------------------------------------------------------------------------------------------------
{
	mutex = SDL_CreateMutex ();
	if (nullptr == mutex)
	{
		std::cout << "Error creating mutex for networkState class [ " << SDL_GetError () << " ]" << std::endl;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Class deconstructor - free mutex
networkState::~networkState ()
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_DestroyMutex (mutex);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the current network textureState
void networkState::setNewState (networkStates newState)
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_LockMutex (mutex);
	previousState = currentState;
	currentState  = newState;
	SDL_UnlockMutex (mutex);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the next textureState
void networkState::setNextState (networkStates newState)
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_LockMutex (mutex);
	nextState = newState;
	SDL_UnlockMutex (mutex);
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the current textureState
networkStates networkState::getCurrentState ()
//----------------------------------------------------------------------------------------------------------------------
{
	networkStates returnValue;

	SDL_LockMutex (mutex);
	returnValue = currentState;
	SDL_UnlockMutex (mutex);

	return returnValue;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return the current textureState of the network
bool networkState::isNetworkStarted ()
//----------------------------------------------------------------------------------------------------------------------
{
	int returnValue;

	SDL_LockMutex (mutex);
	returnValue = networkStarted;
	SDL_UnlockMutex (mutex);

	return returnValue;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the current textureState of the network
void networkState::setNetworkStarted (int newState)
//----------------------------------------------------------------------------------------------------------------------
{
	SDL_LockMutex (mutex);
	networkStarted = newState;
	SDL_UnlockMutex (mutex);
}
