using Godot;
using System;

namespace Game.PlayerWaiting
{
	public partial class TrackerWaiter : Node
	{
		[Signal]
		public delegate void TrackerReadyEventHandler();

		private double WAIT_DURATION_SECS = 1.5;

		private bool _playerOneTracked = false;
		private bool _playerTwoTracked = false;

		private void SetPlayerOneTracked(string action)
		{
			_playerOneTracked = true;
		}

		private void SetPlayerTwoTracked(string action)
		{
			_playerTwoTracked = true;
		}

		public void _OnSkipButtonPressed()
		{
			EmitSignal(SignalName.TrackerReady);
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

			EventBus.Instance.PlayerOneAction += SetPlayerOneTracked;
			EventBus.Instance.PlayerTwoAction += SetPlayerTwoTracked;
		}

		// Called every frame. 'delta' is the elapsed time since the previous frame.
		public override void _Process(double delta)
		{
			if (_playerOneTracked && _playerTwoTracked)
			{
				EmitSignal(SignalName.TrackerReady);
			}
		}

		public override void _ExitTree()
		{
			EventBus.Instance.PlayerOneAction -= SetPlayerOneTracked;
			EventBus.Instance.PlayerTwoAction -= SetPlayerTwoTracked;
		}
	}
}
