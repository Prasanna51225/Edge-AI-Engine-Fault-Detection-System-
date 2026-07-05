import json
import numpy as np
import pandas as pd
from pathlib import Path

from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, confusion_matrix
from sklearn.preprocessing import StandardScaler

import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers

CSV_PATH = "engine_balanced_dataset.csv"
df = pd.read_csv(CSV_PATH)

# Label (what we predict)
TARGET = "MIL"

# Use all columns except label as features
feature_cols = [c for c in df.columns if c != TARGET]

X = df[feature_cols].astype(np.float32).values
y = df[TARGET].astype(np.int32).values

# -----------------------------
# 2) Train / Val / Test split
# -----------------------------
X_train, X_tmp, y_train, y_tmp = train_test_split(
    X, y, test_size=0.30, random_state=42, stratify=y
)
X_val, X_test, y_val, y_test = train_test_split(
    X_tmp, y_tmp, test_size=0.50, random_state=42, stratify=y_tmp
)
print(f"Shapes -> train:{X_train.shape}, val:{X_val.shape}, test:{X_test.shape}")

# -----------------------------
# 3) Scale features (save stats!)
# -----------------------------
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train).astype(np.float32)
X_val   = scaler.transform(X_val).astype(np.float32)
X_test  = scaler.transform(X_test).astype(np.float32)

# Save scaler stats for on-device preprocessing
meta = {
    "feature_order": feature_cols,
    "scaler_mean": scaler.mean_.tolist(),
    "scaler_scale": scaler.scale_.tolist(),
}
Path("artifacts").mkdir(exist_ok=True)
with open("artifacts/feature_meta.json", "w") as f:
    json.dump(meta, f, indent=2)
np.savez("artifacts/scaler_stats.npz", mean=scaler.mean_, scale=scaler.scale_)


tf.random.set_seed(42)

model = keras.Sequential([
    layers.Input(shape=(X_train.shape[1],), name="input"),
    layers.Dense(16, activation="relu", name="dense1"),
    layers.Dense(8, activation="relu", name="dense2"),
    layers.Dense(1, activation="sigmoid", name="output"),
])

model.compile(
    optimizer=keras.optimizers.Adam(1e-3),
    loss="binary_crossentropy",
    metrics=["accuracy"]
)

cb = [
    keras.callbacks.EarlyStopping(
        monitor="val_loss", patience=10, restore_best_weights=True
    ),
    keras.callbacks.ReduceLROnPlateau(
        monitor="val_loss", factor=0.5, patience=5, min_lr=1e-5
    ),
]

history = model.fit(
    X_train, y_train,
    validation_data=(X_val, y_val),
    epochs=200,
    batch_size=256,
    callbacks=cb,
    verbose=1
)

# -----------------------------
# 5) Evaluate
# -----------------------------
y_prob = model.predict(X_test, batch_size=1024).ravel()
y_pred = (y_prob >= 0.5).astype(np.int32)

print("\nClassification report (test):")
print(classification_report(y_test, y_pred, digits=4))

print("Confusion matrix (test):")
print(confusion_matrix(y_test, y_pred))

# -----------------------------
# 6) Export TFLite (float32)
# -----------------------------
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_float = converter.convert()
with open("artifacts/model_float32.tflite", "wb") as f:
    f.write(tflite_float)
print("Saved artifacts/model_float32.tflite")




print(f"Float32 TFLite size: {len(tflite_float)/1024:.1f} KB")
print(f"INT8    TFLite size: {len(tflite_int8)/1024:.1f} KB")

print("\n Training + export complete. Files in ./artifacts")
