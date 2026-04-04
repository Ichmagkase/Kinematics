using Godot;
using System;

public partial class TrackerWaiter : Node
{
	[Signal]
	public delegate void TrackerReadyEventHandler();

	private double WAIT_DURATION_SECS = 1.5;

	private bool _trackerIsReady = false;

	public void SetTrackerReady(string action)
	{
		_trackerIsReady = true;
	}

	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		// Stop _Process from being runned for a brief amount of time,
		// so that the WaitingUntilTracked scene doesn't immediately
		// disappear in case the player tracker is already ready.
		// Otherwise, the transition will look janky.
		SetProcess(false);
		GetTree().CreateTimer(WAIT_DURATION_SECS).Timeout += () =>
		{
			SetProcess(true);
		};

		// If player tracking data starts arriving, then the tracker is ready to go.
		EventBus.Instance.PlayerOneAction += SetTrackerReady;
		EventBus.Instance.PlayerTwoAction += SetTrackerReady;
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
		if (_trackerIsReady)
		{
			EmitSignal(SignalName.TrackerReady);
		}
	}

	public override void _ExitTree()
	{
		EventBus.Instance.PlayerOneAction -= SetTrackerReady;
		EventBus.Instance.PlayerTwoAction -= SetTrackerReady;
	}
}
