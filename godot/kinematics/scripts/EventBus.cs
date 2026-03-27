using Godot;
using System;

public partial class EventBus : Node
{
	[Signal]
	public delegate void PlayerOneGestureEventHandler(string gesture);

	[Signal]
	public delegate void PlayerTwoGestureEventHandler(string gesture);

	public static EventBus Instance { get; private set; }

	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}
}
