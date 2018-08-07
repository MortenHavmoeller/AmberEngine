#pragma once

struct QueueFamilyIndices {
	int transferOnlyFamily = -1;
	int graphicsFamily = -1;
	int presentationFamily = -1;

	bool isComplete() {
		return transferOnlyFamily >= 0 && graphicsFamily >= 0 && presentationFamily >= 0;
	}

	bool hasTransferOnly() {
		return transferOnlyFamily;
	}

	bool hasGraphics() {
		return graphicsFamily >= 0;
	}

	bool hasPresentation() {
		return presentationFamily >= 0;
	}
};
