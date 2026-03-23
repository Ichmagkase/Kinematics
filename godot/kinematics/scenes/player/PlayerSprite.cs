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

		private static String _attack1AnimationName = "attack_1";
		public String Attack1AnimationName {get => _attack1AnimationName;}

		private static String _attack2AnimationName = "attack_2";
		public String Attack2AnimationName {get => _attack2AnimationName;}

		private static bool _playingAttackingAnimation = false;
		public bool PlayingAttackingAnimation {get => _playingAttackingAnimation;}

		private void OnAnimationFinished()
		{
			if (_playingAttackingAnimation)
			{
				_playingAttackingAnimation = false;
				Idle(); 
			}
		}
		public void JumpUp()
		{
			_playingAttackingAnimation = false;
			Play(_jumpUpAnimationName);
		}

		public void JumpDown()
		{
			_playingAttackingAnimation = false;
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
			if (_playingAttackingAnimation) return;
			Play(_runAnimationName);
		}

		public void Attack1() 
		{
			if (_playingAttackingAnimation) return;
			_playingAttackingAnimation = true;
			Play(_attack1AnimationName);
		}

		public void Attack2() 
		{
			if (_playingAttackingAnimation) return;
			_playingAttackingAnimation = true;
			Play(_attack2AnimationName);
		}

		public void Idle()
		{
			if (_playingAttackingAnimation) return;
			Play(_idleAnimationName);
		}

		public StringName GetCurrentAnimationName()
		{
			return Animation;
		}
 
		public override void _Ready()
		{
			Idle();
			AnimationFinished += OnAnimationFinished;
		}
	}
}
