.section .text
.align 4

.global spinlock_acquire
.global spinlock_release

/*
  void spinlock_acquire(spinlock_t *spinlock);
*/
spinlock_acquire:
  cli
  push %esi
  mov 8(%esp), %esi
  lock bts $0, (%esi)
  jnc done

spin:
  sti
  pause
  bt $0, (%esi)
  jc spin

  cli
  lock bts $0, (%esi)
  jnc spin

done:
  pop %esi
  sti
  ret

/* 
  void spinlock_release(spinlock_t *spinlock);
*/
.global spinlock_release
spinlock_release:
  cli
  mov 4(%esp), %esi
  movl $0, (%esi)
  sti
  ret
