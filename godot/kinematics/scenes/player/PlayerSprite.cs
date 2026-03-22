using System;
using Godot;

namespace Game.Player
{
	public partial class PlayerSprite : AnimatedSprite2D
	{
		private static String _jumpUpAnimationName = "jump_up";
		public String JumpUpAnimationName {get => _jumpUpAnimationName;}

		private static String _jumpDownAnimationName = "jump_down";
		public String JumpDownAnimationName {get => _jumpDownAnimationName;}

		private static String _idleAnimationName = "idle";
		public String IdleAnimationName {get => _idleAnimationName;}

		private static String _runAnimationName = "run";
		public String RunAnimationName {get => _runAnimationName;}

		public void JumpUp()
		{
			Play(_jumpUpAnimationName);
		}

		public void JumpDown()
		{
		Play(_jumpDownAnimationName);
		}

		public void FaceLeft()
		{
			FlipH = true;
		}

		public bool IsFacingLeft()
		{
			return FlipH == true;
		}

		public void FaceRight()
		{
			FlipH = false;
		}

		public bool IsFacingRight()
		{
			return FlipH == false;
		}

		public void Run() 
		{
			Play(_runAnimationName);
		}

		public void Idle()
		{
			Play(_idleAnimationName);
		}

		public StringName GetCurrentAnimationName()
		{
			return Animation;
		}


		public override void _Ready()
		{
			Idle();
		}

		public override void _Process(double delta)
		{
		}
	}
}
