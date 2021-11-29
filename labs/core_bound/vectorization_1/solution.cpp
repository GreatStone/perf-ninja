#include "solution.hpp"
#include <cassert>
#include <type_traits>
#include <iostream>

// The alignment algorithm which computes the alignment of the given sequence
// pairs.
result_t compute_alignment(std::vector<sequence_t> const &sequences1,
                           std::vector<sequence_t> const &sequences2) {
  result_t result{};

  for (size_t sequence_idx = 0; sequence_idx < sequences1.size(); ++sequence_idx) {

    using score_t = int16_t;
    using column_t = std::array<score_t, sequence_size_v + 1>;

    sequence_t const &sequence1 = sequences1[sequence_idx];
    sequence_t const &sequence2 = sequences2[sequence_idx];

    /*
     * Initialise score values.
     */
    const score_t gap_open{-11};
    const score_t gap_extension{-1};
    const score_t match{6};
    const score_t mismatch{-4};

    /*
     * Setup the matrix.
     * Note we can compute the entire matrix with just one column in memory,
     * since we are only interested in the last value of the last column in the
     * score matrix.
     */
    column_t score_column{};
    column_t horizontal_gap_column{};
    std::array<score_t, sequence_size_v + 2> last_last_vertical_gap_arr;

    /*
     * Initialise the first column of the matrix.
     */
    horizontal_gap_column[0] = gap_open;
    for (size_t i = 0; i < last_last_vertical_gap_arr.size(); ++i) {
      last_last_vertical_gap_arr[i] = score_t(gap_open + i * gap_extension);
    }

    for (size_t i = 1; i < score_column.size(); ++i) {
      score_column[i] = last_last_vertical_gap_arr[i - 1];
      horizontal_gap_column[i] = last_last_vertical_gap_arr[i - 1] + gap_open;
    }

    // score_column      = 0        gap_open                 gap_open + gap_extension     ... gap_open + i * gap_extension
    // last_vertical_gap = gap_open gap_open + gap_extension gap_open + 2 * gap_extension ... gap_open + (i + 1) * gap_extension

    // Cache arrays
    std::array<score_t, sequence_size_v + 2> last_vertical_gap_arr;
    std::array<score_t, sequence_size_v + 1> last_score_column;

    /*
     * Compute the main recursion to fill the matrix.
     */
    for (unsigned col = 1; col <= sequence2.size(); ++col) {
      last_score_column = score_column;

      for (size_t row = 1; row <= sequence1.size(); ++row) {
        //last_last_vertical_gap_arr[row] = max(score_t(last_vertical_gap_arr[row - 1] + gap_extension), score_t(score_column[row] + gap_open));
      }

      score_column[0] = horizontal_gap_column[0];
      last_last_vertical_gap_arr[0] = score_column[0] + gap_open;
      last_vertical_gap_arr[0] = score_column[0] + gap_open;

      for (unsigned row = 1; row <= sequence1.size(); ++row) {
        //std::cout << "last_vertical_gap_arr[row - 1] = " << last_vertical_gap_arr[row - 1] << '\n';
        //std::cout << "last_last_vertical_gap_arr[row - 1] = " << last_last_vertical_gap_arr[row - 1] << '\n';

        // Compute next score from diagonal direction with match/mismatch.
        score_t best_cell_score = last_score_column[row - 1] + (sequence1[row - 1] == sequence2[col - 1] ? match : mismatch); // ok

        // Determine best score from diagonal, vertical, or horizontal direction.
        best_cell_score = max(best_cell_score, last_vertical_gap_arr[row - 1]);
        best_cell_score = max(best_cell_score, horizontal_gap_column[row]); // ok
        
        // Cache next diagonal value and store optimum in score_column.
        score_column[row] = best_cell_score;

        // Store optimum between gap open and gap extension.
        last_vertical_gap_arr[row] = max(score_t(last_vertical_gap_arr[row - 1] + gap_extension), score_t(score_column[row] + gap_open));
      }

      // Its ok for index 0 as gap_open < gap_extension and score_column[0] == horizontal_gap_column[0].
      for (unsigned row = 0; row <= horizontal_gap_column.size(); ++row) {
        horizontal_gap_column[row] = max(horizontal_gap_column[row] + gap_extension, score_column[row] + gap_open);
      }
    }

    // Report the best score.
    result[sequence_idx] = score_column.back();
  }

  return result;
}
