using Godot;
using System;

public partial class EventBus : Node
{
	[Signal]
	public delegate void PlayerOneActionEventHandler(string action);

	[Signal]
	public delegate void PlayerTwoActionEventHandler(string action);

	public static EventBus Instance { get; private set; }

	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
        if (Instance == null)
        {
            Instance = this;
        }
        else
        {
            QueueFree();
        }
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}
}