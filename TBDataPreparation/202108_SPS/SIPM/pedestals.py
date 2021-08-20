import numpy as np
from matplotlib import pyplot as plt
import numba as nb
import sys, mplhep
from numba_stats import norm_pdf
from scipy.stats import norm

plt.style.use(mplhep.style.ATLAS)

GUI = False

if "plot" in sys.argv:
    GUI = True


@nb.njit(fastmath=True)
def gauss(x, par):
    return par[0], par[0] * norm_pdf(x, par[1], par[2])


with np.load(sys.argv[1]) as f:
    data = np.float32(f["hg"])

row, col, evts = data.shape
pedestals = np.empty((20, 16))

if GUI:
    fig, ax = plt.subplots()

for r in range(row):
    if r % 4 == 0:
        print()
    for c in range(col):
        channeldata = data[r, c, :]
        channeldata = channeldata[channeldata > 0]
        if np.any((channeldata > 0) & (channeldata < 500)) == False:
            print(f"Missing channel {r}-{c}")
            continue
        y, x = np.histogram(channeldata, np.arange(1, 500))
        x = (x[1:] + x[:-1]) * 0.5

        aEstim = y.max()
        muEstim = x[np.argmax(y)]
        sigmaEstim = np.std(channeldata[channeldata < 80])

        dataToFit = channeldata[np.abs(channeldata - muEstim) < 3 * sigmaEstim]
        sigmaEstim = np.std(dataToFit)

        fit = norm.fit(dataToFit, loc=muEstim, scale=sigmaEstim)

        mu, sigma = fit
        pedestals[r, c] = mu

        print(f"Pedestal of fiber {r}-{c} = {mu}")
        if GUI:
            ax.step(x, y, color="k", label=(f"Channel in row {r} column {c}"))
            ax.plot(x, dataToFit.size * norm_pdf(x, mu, sigma), "r")
            mplhep.label._exp_label(ax=ax, data=True, exp="IDEA Dual-Readout", rlabel="SiPM Calibration")
            ax.set_xlim(-100, 500)
            ax.set_yscale("log")
            ax.set_ylim(1, y.max() * 1.3)
            plt.legend(
                frameon=False,
                title=f"$\mu = {mu:.2f}$\n" f"$\sigma = {sigma:.2f}$",
            )
            plt.draw()
            plt.waitforbuttonpress()
            ax.cla()

if "save" in sys.argv:
    np.save("pedestalsHg", pedestals)
