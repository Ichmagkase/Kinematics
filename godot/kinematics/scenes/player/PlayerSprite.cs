using System;
using Godot;

namespace Game.Player
{
	public partial class PlayerSprite : AnimatedSprite2D
	{
		private static String _jumpUpAnimationName = "jump_up";
		public String jumpUpAnimationName {get => _jumpUpAnimationName;}

		private static String _jumpDownAnimationName = "jump_down";
		public String jumpDownAnimationName {get => _jumpDownAnimationName;}

		private static String _idleAnimationName = "jump_down";
		public String idleAnimationName {get => _idleAnimationName;}

		public void JumpUp()
		{
			Play(_jumpUpAnimationName);
		}

		public void JumpDown()
		{
			Play(_jumpDownAnimationName);
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
