#include <iostream>
#include <cstdint>
#include <climits>

// Shoutout to ChatGPT for creating this working code on the first try!
// Simulate behavior of FMAC in FMAC_FUNC_IIR_DIRECT_FORM_1 mode
// Some rounding errors on the test data have been observed

#define B0 (0x1B37)
#define B1 (0xE)
#define B2 (0xE4D7)
#define A1 (0x6000)
#define A2 (0xE000)
#define POST_SHIFT (1)

static int16_t x1, x2;
static int16_t y1, y2;
static int32_t acc;

static int16_t test_inputs[16] = {3721, 158, 4092, 2876, 941, 2045, 3782, 67, 3156, 2299, 124, 3998, 1777, 845, 2560, 310};

int16_t process_sample(int16_t x_q15)
    {
        // Local copies of coefficients as signed 32-bit (so negative values sign-extend correctly)
        const int32_t b0 = static_cast<int16_t>(B0);
        const int32_t b1 = static_cast<int16_t>(B1);
        const int32_t b2 = static_cast<int16_t>(B2);
        const int32_t a1 = static_cast<int16_t>(A1);
        const int32_t a2 = static_cast<int16_t>(A2);

        // Products:
        // Q1.15 * Q1.15 = Q2.30
        // To align to accumulator Q4.22 we must shift right by (30 - 22) = 8 bits.
        // Use 64-bit for multiply to avoid overflow, then arithmetic shift >> 8.
        auto mul_q15 = [](int32_t coef_q15, int32_t sample_q15) -> int32_t {
            int64_t prod = static_cast<int64_t>(coef_q15) * static_cast<int64_t>(sample_q15); // Q2.30
            // arithmetic shift with rounding omitted here (we will round only at final conversion)
            int64_t aligned = prod >> 8; // now effectively Q2.22 (same fractional bits as Q4.22)
            return static_cast<int32_t>(aligned); // safe: aligned fits in 32 bits given ranges
        };


        // Compute feedforward terms (B * x)
        int32_t t_b0 = mul_q15(b0, x_q15);   // Q?.22 stored in int32 (fractional = 22)
        int32_t t_b1 = mul_q15(b1, x1);      // uses previous input x1 (Q1.15)
        int32_t t_b2 = mul_q15(b2, x2);

        // Compute feedback terms (A * y_previous)
        // Note: IIR difference equation usually: y[n] = (b0 x[n] + b1 x[n-1] + b2 x[n-2] - a1 y[n-1] - a2 y[n-2])
        int32_t t_a1 = mul_q15(a1, y1);
        int32_t t_a2 = mul_q15(a2, y2);

        // Accumulate into accumulator (Q4.22). No saturation — wrap allowed.
        // acc may also be used for other algorithms; here we recompute fresh each sample.
        // If you prefer incremental accumulation, adapt accordingly.
        int32_t local_acc = 0;
        // add feedforward
        local_acc += t_b0;
        local_acc += t_b1;
        local_acc += t_b2;
        // subtract feedback (note: coefficients A1/A2 include the sign convention)
        local_acc += t_a1;
        local_acc += t_a2;

        // Apply post left shift (0..7)
        constexpr int POST = POST_SHIFT;
        static_assert(POST >= 0 && POST <= 7, "POST_SHIFT must be between 0 and 7.");
        // shift left without saturation — may wrap in 32-bit signed arithmetic (as per spec)
        int32_t shifted = (POST == 0) ? local_acc : (local_acc << POST);

        // Convert from Q4.22 -> Q1.15 by shifting right (22 - 15) = 7 bits.
        // Perform rounding (signed): add rounding offset depending on sign.
        const int SHIFT_TO_Q15 = 7;
        int32_t round_offset = (shifted >= 0) ? (1 << (SHIFT_TO_Q15 - 1)) : - (1 << (SHIFT_TO_Q15 - 1));
        int32_t rounded = (shifted + round_offset) >> SHIFT_TO_Q15; // now Q1.15 (still in int32)

        // Cast to int16_t for output (wraps on overflow — as requested no saturation in accumulator;
        // here we choose wrapping behaviour for the final 16-bit output as well).
        int16_t y_q15 = static_cast<int16_t>(rounded & 0xFFFF);

        // Update delay lines: inputs and outputs (store as Q1.15)
        x2 = x1;
        x1 = x_q15;

        y2 = y1;
        y1 = y_q15;

        // store last accumulator value (optional)
        acc = local_acc;

        return y_q15;
    }


int main (void)
{
    for (size_t i = 0; i < 16; i++) {
        int16_t output = process_sample(test_inputs[i]);
        std::cout << "Result: " << output << std::endl;
    }

}
