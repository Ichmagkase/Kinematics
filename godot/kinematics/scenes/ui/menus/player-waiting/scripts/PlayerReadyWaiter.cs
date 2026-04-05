using Godot;
using System;

public partial class PlayerReadyWaiter : Node
{
	[Signal]
	public delegate void AllPlayersReadyEventHandler();

	[Export]
	public Label PlayerOneReadyLabel;

	[Export]
	public Label PlayerTwoReadyLabel;

	private string READY_ACTION = "block";
	private string READY_LABEL = "Ready";

	private bool _playerOneReady = false;
	private bool _playerTwoReady = false;

	public void PlayerOneActionHandler(string action)
	{
		if (action == READY_ACTION)
		{
			_playerOneReady = true;
			PlayerOneReadyLabel.Text = READY_LABEL;
		}
	}

	public void PlayerTwoActionHandler(string action)
	{
		if (action == READY_ACTION)
		{
			_playerTwoReady = true;
			PlayerTwoReadyLabel.Text = READY_LABEL;
		}
	}

	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		EventBus.Instance.PlayerOneAction += PlayerOneActionHandler;
		EventBus.Instance.PlayerTwoAction += PlayerTwoActionHandler;
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
		if (_playerOneReady && _playerTwoReady)
		{
			EmitSignal(SignalName.AllPlayersReady);
		}
	}

	public override void _ExitTree()
	{
		EventBus.Instance.PlayerOneAction -= PlayerOneActionHandler;
		EventBus.Instance.PlayerTwoAction -= PlayerTwoActionHandler;
	}
}
