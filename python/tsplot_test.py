from tsplot import plot_ts, plot_multiple_ts, is_1d

import numpy as np
import unittest
from unittest.mock import patch


class TestPlots(unittest.TestCase):

    def setUp(self):
        self.ts2d1 = np.array([[0.14268096, 0.16352641, 0.31639171, 0.50144441, 0.62164556],
                               [0.14278092, 0.20342093, 0.30996117, 0.56791866, 0.72610382],
                               [0.13264092, 0.18700444, 1.00000000, 0.51986262, 0.61111458],
                               [0.14740364, 0.18013568, 0.34679930, 0.51867516, 0.63013684],
                               [0.09827740, 0.19056831, 0.33077784, 0.46010118, 0.49291126]])

        self.ts2d2 = np.array([[0.33077019, 0.47714208, 0.51865324, 0.27678448, 0.12099174],
                               [0.52401640, 0.63254814, 0.60641147, 0.21836176, 0.07288730],
                               [0.63551627, 0.79059511, 1.00000000, 0.07013124, 0.03739288],
                               [0.72618183, 0.74771765, 0.11639440, 0.04274514, 0.01505505],
                               [0.69037351, 0.53182554, 0.09496877, 0.00998256, 0.00000000]])

        self.ts1d1 = np.array([0.63551627, 0.79059511, 1.00000000, 0.07013124, 0.03739288])

    def test_is_1d(self):
        self.assertTrue(is_1d(np.array([1, 2, 3, 4])))
        self.assertTrue(is_1d(np.array([[1, 2, 3, 4]])))
        self.assertFalse(is_1d(np.zeros((2, 2))))

    @patch("tsplot.plot_ts_2d")
    @patch("tsplot.plot_ts_1d")
    def test_dispatching_single_plot(self, mock_plot_ts_1d, mock_plot_ts_2d):
        plot_ts(self.ts1d1)
        mock_plot_ts_1d.assert_called()
        mock_plot_ts_2d.assert_not_called()

        mock_plot_ts_1d.reset_mock()
        mock_plot_ts_2d.reset_mock()

        plot_ts(self.ts2d1)
        mock_plot_ts_1d.assert_not_called()
        mock_plot_ts_2d.assert_called()

    @patch("tsplot.plt")
    @patch("tsplot.plot_ts_2d")
    @patch("tsplot.plot_ts_1d")
    def test_dispatching_multiple_plots(self, mock_plot_ts_1d, mock_plot_ts_2d, mock_plt):

        plot_multiple_ts([self.ts1d1] * 10)
        self.assertEqual(mock_plot_ts_1d.call_count, 10)
        mock_plot_ts_2d.assert_not_called()
        mock_plt.show.assert_called_once()

        mock_plot_ts_1d.reset_mock()
        mock_plot_ts_2d.reset_mock()
        mock_plt.reset_mock()

        plot_multiple_ts([self.ts2d1] * 10)
        mock_plot_ts_1d.assert_not_called()
        self.assertEqual(mock_plot_ts_2d.call_count, 10)
        mock_plt.show.assert_called_once()
