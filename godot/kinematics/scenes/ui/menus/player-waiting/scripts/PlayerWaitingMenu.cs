using Godot;
using System;

public partial class PlayerWaitingMenu : Node
{
	[Export]
	public PackedScene WaitingPlayerReadyMenu;

	private TrackerWaiter _trackerWaiter;
	private PlayerReadyWaiter _playerReadyWaiter;

	private void TrackerReadyHandler()
	{
		_trackerWaiter.TrackerReady -= TrackerReadyHandler;
		_trackerWaiter.QueueFree();
		ActivateWaitingPlayerReadyScene();
	}

	private void AllPlayersReadyHandler()
	{
		_playerReadyWaiter.AllPlayersReady -= AllPlayersReadyHandler;
		_playerReadyWaiter.QueueFree();
		ActivateCountdownScene();
	}

	private void ActivateWaitingPlayerReadyScene()
	{
		_playerReadyWaiter = WaitingPlayerReadyMenu.Instantiate<PlayerReadyWaiter>();
		AddChild(_playerReadyWaiter);
		_playerReadyWaiter.AllPlayersReady += AllPlayersReadyHandler;
	}

	private void ActivateCountdownScene()
	{
		// Create countdown scene
	}

	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		_trackerWaiter = GetNode<TrackerWaiter>("WaitingUntilTracked");
		_trackerWaiter.TrackerReady += TrackerReadyHandler;
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}
}
