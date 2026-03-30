using Godot;
using System.Linq;

namespace Game.Player
{
	public partial class Player : CharacterBody2D
	{
		public int PlayerId;
		private PlayerSprite _playerSprite;
		private PlayerConfig _playerConfig;
		private bool _hasDoubleJumpped = false;
		private readonly string[] attackActions = 
		{
			PlayerInputActions.Attack1,
			PlayerInputActions.Attack2,
		};

		// Since we can receive input from signals and Godot Inputs, we need some protection
		private readonly object _inputLock = new();
		private struct PendingInput
		{
			public bool Jump;
			public bool Attack1;
			public bool Attack2;
			public bool Block;
			public Vector2 Movement;
		}
		private PendingInput _pendingInput;

		public void HandleActionSignal(string e)
		{
			GD.Print($"PlayerId {PlayerId}: {e}");

			// The logic will be handled in _PhysicsProcess()
			{
				switch (e)
				{
					case PlayerInputActions.Jump:      _pendingInput.Jump = true;                    break;
					case PlayerInputActions.MoveLeft:  _pendingInput.Movement += new Vector2(-1, 0); break;
					case PlayerInputActions.MoveRight: _pendingInput.Movement += new Vector2(1, 0);  break;
					case PlayerInputActions.MoveDown:  _pendingInput.Movement += new Vector2(0, 1);  break;
					case PlayerInputActions.Attack1:   _pendingInput.Attack1 = true;                 break;
					case PlayerInputActions.Attack2:   _pendingInput.Attack2 = true;                 break;
					case PlayerInputActions.Block:     _pendingInput.Block = true;                   break;
				}
			}
		}

		public override void _Ready()
		{
			if (PlayerId <= 0)
			{
				GD.PrintErr($"PlayerId not set (Id {PlayerId}, expected > 0). This should be done by the GameStart.cs script");
			}
			_playerConfig = GetNode<PlayerConfig>("PlayerConfig");
			_playerSprite = GetNode<PlayerSprite>("AnimatedSprite2D");
		}

		public override void _PhysicsProcess(double delta)
		{
			HandleGravity(delta);
			lock (_inputLock);

			// Merge Godot inputs and signals
			bool jump = Input.IsActionJustPressed(PlayerInputActions.Jump) || _pendingInput.Jump;

			Vector2 keyboardDir = Input.GetVector(
				PlayerInputActions.MoveLeft, PlayerInputActions.MoveRight,
				PlayerInputActions.MoveUp,   PlayerInputActions.MoveDown);
			Vector2 movement = keyboardDir + _pendingInput.Movement;

			string? triggeredAction =
				attackActions.FirstOrDefault(a => Input.IsActionPressed(a))
				?? (_pendingInput.Attack1 ? PlayerInputActions.Attack1 : null)
				?? (_pendingInput.Attack2 ? PlayerInputActions.Attack2 : null);

			bool block = Input.IsActionJustPressed(PlayerInputActions.Block) || _pendingInput.Block
					|| _playerSprite.GetCurrentAnimationName() == _playerSprite.BlockAnimationName;

			// clear signal state for next frame
			_pendingInput = default;

			if (jump) HandleJump();
			HandleMovement(movement);
			if (triggeredAction != null) HandleAttack(triggeredAction);
			if (block) HandleBlock();

			UpdateAnimation();
			MoveAndSlide();
		}

		private void HandleGravity(double delta)
		{
			if (!IsOnFloor())
				Velocity += GetGravity() * (float)delta;
		}

		private void HandleJump()
		{
			bool canJump = IsOnFloor();
			bool canDoubleJump = !IsOnFloor()
				&& !_hasDoubleJumpped
				&& _playerConfig.CanDoubleJump;

			if (!canJump && !canDoubleJump) return;

			_hasDoubleJumpped = !canJump; // true if double jumping, false if normal jump

			Velocity = new Vector2(Velocity.X, _playerConfig.JumpVelocity);
		}

		private void HandleAttack(string action)
		{
			if (!IsOnFloor()) return;
			if (action == PlayerInputActions.Attack1)
			{
				_playerSprite.Attack1();
				return;
			}
			if  (action == PlayerInputActions.Attack2)
			{
				_playerSprite.Attack2();
				return;
			}
		}

		private void HandleBlock()
		{
			if (!IsOnFloor() || Velocity != Vector2.Zero) return;
			if (Input.IsActionJustPressed(PlayerInputActions.Block))
			{
				_playerSprite.Block();
				return;
			}
			else if (Input.IsActionPressed(PlayerInputActions.Block) && _playerSprite.GetCurrentAnimationName() == _playerSprite.BlockAnimationName)
			{
				return;
	
			} 
			else if (_playerSprite.GetCurrentAnimationName() == _playerSprite.BlockAnimationName)
			{
				_playerSprite.EndBlock();
			}
		}

		private void HandleMovement(Vector2 direction)
		{
			if (direction != Vector2.Zero)
				Velocity = new Vector2(direction.X * _playerConfig.Speed, Velocity.Y);
			else
				Velocity = new Vector2(Mathf.MoveToward(Velocity.X, 0, _playerConfig.Speed), Velocity.Y);
		}

		private void UpdateAnimation()
		{
			bool onFloor = IsOnFloor();
			bool isFalling = !onFloor && Velocity.Y >= 0;
			bool isRising = !onFloor && Velocity.Y < 0;
			bool isMoving = Velocity.X != 0;

			if (isFalling)  { _playerSprite.JumpDown(); return; }
			if (isRising)   { _playerSprite.JumpUp();   return; }
			if (isMoving)
			{
				if (Velocity.X < 0) _playerSprite.FaceLeft();
				else if (Velocity.X > 0) _playerSprite.FaceRight();
				_playerSprite.Run();
				return;
			}

			_playerSprite.Idle();
		}
	}
}
