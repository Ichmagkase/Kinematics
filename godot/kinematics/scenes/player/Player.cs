using Godot;

namespace Game.Player
{
	public partial class Player : CharacterBody2D
	{		

		private PlayerSprite _playerSprite;

		private Vector2 HandlePlayerJumpUp(Vector2 currentVector)
		{
			currentVector.Y = PlayerConfig.JumpVelocity;
			_playerSprite.JumpUp();
			return currentVector;
		}
	
		private Vector2 HandlePlayerJumpDown(Vector2 currentVector, double delta)
		{
			_playerSprite.JumpDown();
			return currentVector;
		}

		public void HandlePlayerCollideWithFloor()
		{
			if (_playerSprite.GetCurrentAnimationName() == _playerSprite.jumpDownAnimationName)
			{
				_playerSprite.Idle();
			}
		}
		
		public override void _Ready()
		{
			_playerSprite = GetNode<PlayerSprite>("AnimatedSprite2D");
		}

		public override void _PhysicsProcess(double delta)
		{
			Vector2 velocity = Velocity;

			if (!IsOnFloor())
			{
				velocity += GetGravity() * (float)delta;
			}

			if (!IsOnFloor() && velocity.Y >= 0)
			{
				velocity = HandlePlayerJumpDown(velocity, delta);
			}

			if (IsOnFloor())
			{
				HandlePlayerCollideWithFloor();
			}

			if (Input.IsActionJustPressed(PlayerInputActions.Jump) && IsOnFloor())
			{
				velocity = HandlePlayerJumpUp(velocity);
			}

			Vector2 direction = Input.GetVector(
				PlayerInputActions.MoveLeft,
				PlayerInputActions.MoveRight,
				PlayerInputActions.MoveDown,
				PlayerInputActions.Jump
			);

			if (direction != Vector2.Zero)
			{
				velocity.X = direction.X * PlayerConfig.Speed;
			}
			else
			{
				velocity.X = Mathf.MoveToward(Velocity.X, 0, PlayerConfig.Speed);
			}


			Velocity = velocity;
			GD.Print(Velocity);
			MoveAndSlide();
		}
	}
}
