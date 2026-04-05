using Godot;
using System;

namespace Game.PlayerWaiting
{
	public partial class Countdown : Node
	{
		[Signal]
		public delegate void CountdownFinishedEventHandler();

		// When NextCountdownStep is called, it immediately decrements the
		// time left, so e.g. setting _timeLeft to 4 creates a countdown of
		// 3 seconds.
		private int _timeLeftSecs = 4;

		private Label _countdownLabel;

		// The timer should have a wait time of 1.0 sec, which is set
		// in the Godot inspector.
		private Timer _timer;

		private void UpdateAndAnimateLabel(Label label, string newText)
		{
			Tween tween = GetTree().CreateTween();

			label.Scale = new Vector2(0.8f, 0.8f);

			label.Text = newText;

			tween.TweenProperty(label, "scale", new Vector2(1.2f, 1.2f), 0.5f)
				.SetTrans(Tween.TransitionType.Back)
				.SetEase(Tween.EaseType.Out);
		}

		private void NextCountdownStep()
		{
			_timeLeftSecs--;
			if (_timeLeftSecs > 0)
			{
				UpdateAndAnimateLabel(_countdownLabel, _timeLeftSecs.ToString());
			}
			else
			{
				_timer.Timeout -= NextCountdownStep;
				EmitSignal(SignalName.CountdownFinished);
			}
		}

		// Called when the node enters the scene tree for the first time.
		public override void _Ready()
		{
			_countdownLabel = GetNode<Label>("CountdownLabel");
			_timer = GetNode<Timer>("Timer");

			_timer.Timeout += NextCountdownStep;
			_timer.Start();

			NextCountdownStep();
		}

		// Called every frame. 'delta' is the elapsed time since the previous frame.
		public override void _Process(double delta)
		{
		}
	}
}
