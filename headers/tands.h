//
// Created by dors on 10/24/20.
//

#ifndef C379A2_TANDS_H
#define C379A2_TANDS_H
// Trans simulates processing a transaction
// It does not simulate this by using sleep -- that would free up the CPU.
// Thus, Trans does a silly computation to use up CPU cycles. Note that the
// computation has to produce some result. If it doesn't, then a smart
// compiler will notice this and delete (optimize) the code!

// The loop's computation (to waste time) is used to modify TransSave.
// TransSave is added to the wait time in Sleep -- a few billionths of a
// second. By doing this, something "real" comes from the computation, and
// the compiler is fooled.
void Trans( int n );
// Sleep simulates pauses between transactions
// Parameter given expresses wait time in hundreds of a nanosecond.
void Sleep( int n );

#endif //C379A2_TANDS_H
