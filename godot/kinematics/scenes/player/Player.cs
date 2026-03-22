using Godot;

namespace Game.Player
{
	public partial class Player : CharacterBody2D
	{
		private PlayerSprite _playerSprite;

		public override void _Ready()
		{
			_playerSprite = GetNode<PlayerSprite>("AnimatedSprite2D");
		}

		public override void _PhysicsProcess(double delta)
		{
			Vector2 velocity = Velocity;

			velocity = HandleGravity(velocity, delta);
			velocity = HandleJump(velocity);
			velocity = HandleMovement(velocity);

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
			bool canJump = IsOnFloor() && Input.IsActionJustPressed(PlayerInputActions.Jump);
			if (!canJump) return velocity;

			velocity.Y = PlayerConfig.JumpVelocity;
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
				velocity.X = direction.X * PlayerConfig.Speed;
			else
				velocity.X = Mathf.MoveToward(velocity.X, 0, PlayerConfig.Speed);

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
				GD.Print(_playerSprite.IsFacingRight());
				GD.Print(_playerSprite.IsFacingLeft());
				if (velocity.X < 0) 
				{
						_playerSprite.FaceLeft();
						_playerSprite.Play(_playerSprite.RunAnimationName);
				}
				if (velocity.X > 0) 
				{
						_playerSprite.FaceRight();
						_playerSprite.Play(_playerSprite.RunAnimationName);	
				}
				_playerSprite.Play(_playerSprite.RunAnimationName);
				return;
			}

			_playerSprite.Idle();
		}
	}
}
