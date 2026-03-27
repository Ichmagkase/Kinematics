using Godot;

namespace Game.Player
{
	public partial class Player : CharacterBody2D
	{
		private PlayerSprite _playerSprite;
		private PlayerConfig _playerConfig;
		private bool _hasDoubleJumpped = false;

		public override void _Ready()
		{
			_playerConfig = GetNode<PlayerConfig>("PlayerConfig");
			_playerSprite = GetNode<PlayerSprite>("AnimatedSprite2D");
		}

		public override void _PhysicsProcess(double delta)
		{
			Vector2 velocity = Velocity;

			velocity = HandleGravity(velocity, delta);
			velocity = HandleJump(velocity);
			velocity = HandleMovement(velocity);
			velocity = HandleAttack(velocity);
			velocity = HandleBlock(velocity);

			UpdateAnimation(velocity);

			Velocity = velocity;
			MoveAndSlide();
		}

		private Vector2 HandleGravity(Vector2 velocity, double delta)
		{
			if (!IsOnFloor())
				velocity += GetGravity() * (float)delta;

			return velocity;
		}

		private Vector2 HandleJump(Vector2 velocity)
		{
			bool canJump = IsOnFloor() && Input.IsActionPressed(PlayerInputActions.Jump);
			bool canDoubleJump = !IsOnFloor()
								&& Input.IsActionJustPressed(PlayerInputActions.Jump)
								&& !_hasDoubleJumpped
								&& _playerConfig.CanDoubleJump;

			if (!canJump && !canDoubleJump) return velocity;
			else if (!canJump) _hasDoubleJumpped = true; // Don't double jump again
			else _hasDoubleJumpped = false; // Allow double jump if this is the first jump

			if (canJump)
			{
				velocity.Y = _playerConfig.JumpVelocity;
			} 
			else
			{
				velocity.Y = _playerConfig.JumpVelocity;
			}
			return velocity;
		}

		private Vector2 HandleAttack(Vector2 velocity)
		{
			if (!IsOnFloor()) return velocity;
			if (Input.IsActionJustPressed(PlayerInputActions.Attack1))
			{
				_playerSprite.Attack1();
				return velocity;
			}
			if  (Input.IsActionJustPressed(PlayerInputActions.Attack2))
			{
				_playerSprite.Attack2();
				return velocity;
			}
			return velocity;
		}

		private Vector2 HandleBlock(Vector2 velocity)
		{
			if (Input.IsActionJustPressed(PlayerInputActions.Block))
			{
				_playerSprite.Block();
				return velocity;
			}
			else if (Input.IsActionPressed(PlayerInputActions.Block) && _playerSprite.GetCurrentAnimationName() == _playerSprite.BlockAnimationName)
			{
				return velocity;
	
			} 
			else if (_playerSprite.GetCurrentAnimationName() == _playerSprite.BlockAnimationName)
			{
				_playerSprite.EndBlock();
			}

			return velocity;
		}

		private Vector2 HandleMovement(Vector2 velocity)
		{
			Vector2 direction = Input.GetVector(
				PlayerInputActions.MoveLeft,
				PlayerInputActions.MoveRight,
				PlayerInputActions.MoveUp,
				PlayerInputActions.MoveDown
			);

			if (direction != Vector2.Zero)
				velocity.X = direction.X * _playerConfig.Speed;
			else
				velocity.X = Mathf.MoveToward(velocity.X, 0, _playerConfig.Speed);

			return velocity;
		}

		private void UpdateAnimation(Vector2 velocity)
		{
			bool onFloor = IsOnFloor();
			bool isFalling = !onFloor && velocity.Y >= 0;
			bool isRising = !onFloor && velocity.Y < 0;
			bool isMoving = velocity.X != 0;

			if (isFalling)  { _playerSprite.JumpDown(); return; }
			if (isRising)   { _playerSprite.JumpUp();   return; }
			if (isMoving)
			{
				if (velocity.X < 0) 
				{
						_playerSprite.FaceLeft();
						_playerSprite.Run();
				}
				if (velocity.X > 0) 
				{
						_playerSprite.FaceRight();
						_playerSprite.Run();
				}
				_playerSprite.Run();
				return;
			}

			_playerSprite.Idle();
		}
	}
}
