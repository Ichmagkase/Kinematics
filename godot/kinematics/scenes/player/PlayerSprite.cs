using System;
using System.Linq;
using Godot;

namespace Game.Player
{
	public partial class PlayerSprite : AnimatedSprite2D
	{
		private AudioStreamPlayer2D soundEffectPlayer;

		private static String _jumpUpAnimationName = "jump_up";
		public String JumpUpAnimationName {get => _jumpUpAnimationName;}

		private static String _jumpDownAnimationName = "jump_down";
		public String JumpDownAnimationName {get => _jumpDownAnimationName;}

		private static String _idleAnimationName = "idle";
		public String IdleAnimationName {get => _idleAnimationName;}

		private static String _deathAnimationName = "death";
		public String DeathAnimationName {get => _deathAnimationName;}

		private static String _runAnimationName = "run";
		public String RunAnimationName {get => _runAnimationName;}

		private static String _hitAnimationName = "hit";
		public String HitAnimationName {get => _hitAnimationName;}

		private static String _attack1AnimationName = "attack_1";
		public String Attack1AnimationName {get => _attack1AnimationName;}

		private static String _attack2AnimationName = "attack_2";
		public String Attack2AnimationName {get => _attack2AnimationName;}

		private static String _blockAnimationName = "block";
		public String  BlockAnimationName {get => _blockAnimationName;}

		private bool _playingAttackingAnimation = false;
		public bool PlayingAttackingAnimation {get => _playingAttackingAnimation;}

		private bool _playingBlockingAnimation = false;
		public bool PlayingBlockingAnimation {get => _playingBlockingAnimation;}

		private bool _playingDeathAnimation = false;
		public bool PlayingDeathAnimation {get => _playingDeathAnimation;}

		private bool _playingHitAnimation = false;
		public bool PlayingHitAnimation {get => _playingHitAnimation;}

		private void OnAnimationFinished()
		{
			if (_playingAttackingAnimation)
			{
				_playingAttackingAnimation = false;
				Idle(); 
			}
			if (_playingDeathAnimation)
			{
				var nextScene = GD.Load<PackedScene>(GlobalConfig.Instance.HomeScenePath);
				GetTree().ChangeSceneToPacked(nextScene);
			}
			if (_playingHitAnimation)
            {
                _playingHitAnimation = false;
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
			if (_playingAttackingAnimation || _playingBlockingAnimation) return;
			Play(_runAnimationName);
		}

		public void Attack1() 
		{
			if (_playingAttackingAnimation || _playingBlockingAnimation) return;
			_playingAttackingAnimation = true;
			Play(_attack1AnimationName);
			playSound(_attack1AnimationName);

		}

		public void Attack2() 
		{
			if (_playingAttackingAnimation || _playingBlockingAnimation) return;
			_playingAttackingAnimation = true;
			Play(_attack2AnimationName);
			playSound(_attack1AnimationName);
			
		}

		public void Block() 
		{
			_playingAttackingAnimation = false;
			_playingBlockingAnimation = true;
			Play(_blockAnimationName);
		}

		public void EndBlock()
        {
            _playingBlockingAnimation = false;
			Idle();
        }

		public void Idle()
		{
			if (_playingAttackingAnimation || _playingBlockingAnimation || _playingHitAnimation) return;
			Play(_idleAnimationName);
		}

		public void Hit()
		{
			if (_playingAttackingAnimation) return;
			_playingHitAnimation = true;
			Play(_hitAnimationName);
		}

		public void Death()
		{
			_playingAttackingAnimation = false;
			_playingBlockingAnimation = false;
			_playingHitAnimation = false;
			_playingDeathAnimation = true;
			Play(_deathAnimationName);
		}

		public StringName GetCurrentAnimationName()
		{
			return Animation;
		}

		private void playSound(string action)
        {
			soundEffectPlayer.Stream = (AudioStream)GD.Load($"{GlobalConfig.Instance.PlayerSoundEffectPath}/{action}.wav");			
			soundEffectPlayer.Play();
        }
 
		public override void _Ready()
		{
			Idle();
			AnimationFinished += OnAnimationFinished;
			soundEffectPlayer = GetParent().GetNode<AudioStreamPlayer2D>("AudioStreamPlayer2D");
			if (soundEffectPlayer == null)
            {
				GD.PrintErr("failed to find AudioStreamPlayer2D for spirtes");
            }
		}
	}
}
