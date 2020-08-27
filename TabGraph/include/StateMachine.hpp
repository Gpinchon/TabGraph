#pragma once

class State;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();
	State *CurrentState();
	
};